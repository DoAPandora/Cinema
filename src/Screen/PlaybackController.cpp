#include "main.hpp"
#include "Screen/PlaybackController.hpp"
#include "Hooks/LevelData.hpp"
#include "Video/VideoLoader.hpp"
#include "bs-events/shared/BSEvents.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/SceneInfo.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"

DEFINE_TYPE(Cinema, PlaybackController);

using namespace UnityEngine;
using namespace Video;

namespace Cinema {
    
    PlaybackController* PlaybackController::instance = nullptr;
    PlaybackController* PlaybackController::get_instance() { return instance; }
    void PlaybackController::Create()
    {
        if(instance && instance->m_CachedPtr.m_value)
            return;
        GameObject::New_ctor("CinemaPlaybackController")->AddComponent<PlaybackController*>();
    }

    void PlaybackController::Start()
    {
        if(instance && instance->m_CachedPtr.m_value)
        {
            Object::Destroy(this);
            return;
        }
        instance = this;
        INFO("Created PlaybackController");

        GameObject::DontDestroyOnLoad(this->get_gameObject());
        videoPlayer = get_gameObject()->AddComponent<CustomVideoPlayer*>();
        videoPlayer->get_gameObject()->SetActive(false);
        videoPlayer->set_sendFrameReadyEvents(true);

        prepareCompleted = {&PlaybackController::OnPrepareComplete, this};
        videoPlayer->player->prepareCompleted += prepareCompleted;
        frameReady = {&PlaybackController::FrameReady, this};
        videoPlayer->player->frameReady += frameReady;

        BSEvents::songPaused += {&PlaybackController::PauseVideo, this};
        BSEvents::songUnpaused += {&PlaybackController::ResumeVideo, this};
        BSEvents::menuSceneLoaded += {&PlaybackController::OnMenuSceneLoaded, this};
        BSEvents::lateMenuSceneLoadedFresh += {&PlaybackController::OnMenuSceneLoadedFresh, this};
        BSEvents::gameSceneActive += {&PlaybackController::GameSceneActive, this};
        BSEvents::gameSceneLoaded += {&PlaybackController::GameSceneLoaded, this};

        OnMenuSceneLoadedFresh(nullptr);
    }

    void PlaybackController::Update()
    {
    }

    void PlaybackController::ResumeVideo()
    {
        if(!getModConfig().enabled.GetValue() || videoPlayer->get_isPlaying())
            return;

        float referenceTime = GetReferenceTime();
        if(referenceTime > 0)
        {
            videoPlayer->Play();
        }
        else
            StartCoroutine(prepareVideoCoroutine);
    }

    void PlaybackController::PauseVideo()
    {
        StopAllCoroutines();
        if(videoPlayer->get_isPlaying())
        {
            videoPlayer->Pause();
        }
    }

    float PlaybackController::GetReferenceTime(float referenceTime, float playbackSpeed)
    {
        if(!activeAudioSource)
            return 0;

        float time;
        if(referenceTime == 0 && activeAudioSource->get_time() == 0)
            time = lastKnownAudioSourceTime;
        else
            time == referenceTime == 0 ? activeAudioSource->get_time() : referenceTime;

        float speed = playbackSpeed == 0 ? videoConfig->playbackSpeed : playbackSpeed;
        
        return time * speed + videoConfig->offset / 1000;
    }

    void PlaybackController::GameSceneActive()
    {
        auto data = LevelData::get_currentGameplayCoreData();
        if(!data)
            return;

        StringW sceneName = data->environmentInfo->sceneInfo->sceneName;
        auto scene = SceneManagement::SceneManager::GetSceneByName(sceneName);
		/* Crashes on next scene transition */
        // if(scene.IsValid())
        //     SceneManagement::SceneManager::MoveGameObjectToScene(get_gameObject(), scene);
        INFO("Moving to game scene");
    }

    void PlaybackController::GameSceneLoaded()
    {
        get_gameObject()->SetActive(true);
        DEBUG("GameSceneLoaded");
        StopAllCoroutines();

        if(!getModConfig().enabled.GetValue())
        {
            INFO("Mod disabled");
            return;
        }

        videoPlayer->get_gameObject()->SetActive(false);

        auto data = LevelData::get_currentGameplayCoreData();
        if(!data)
        {
            INFO("LevelData was null");
            return;
        }

        auto level = data->previewBeatmapLevel;
        auto config = VideoLoader::GetConfigForLevel(level);
        if(!config.has_value())
        {
            videoPlayer->get_gameObject()->SetActive(false);
            INFO("Could not find config for level");
            return;
        }

        SetSelectedLevel(level, config.value());
        if(!config->get_isPlayable())
            return;

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(PlayVideoAfterAudioSourceCoroutine(false)));
    }

    custom_types::Helpers::Coroutine PlaybackController::PlayVideoAfterAudioSourceCoroutine(bool preview)
    {
        float startTime;
        DEBUG("PlayAfterAudioSource");

        if(!preview)
        {
            GlobalNamespace::AudioTimeSyncController* timeSyncController;
            while(timeSyncController = Resources::FindObjectsOfTypeAll<GlobalNamespace::AudioTimeSyncController*>().FirstOrDefault(), !timeSyncController)
            {
                co_yield nullptr;
            }

            audioTimeSyncController = Resources::FindObjectsOfTypeAll<GlobalNamespace::AudioTimeSyncController*>().FirstOrDefault([](GlobalNamespace::AudioTimeSyncController* x) { return x->get_transform()->get_parent()->get_parent()->get_name()->Contains("StandardGameplay"); });
            activeAudioSource = audioTimeSyncController->audioSource;

            if(activeAudioSource)
            {
                lastKnownAudioSourceTime = 0;
                while(!activeAudioSource->get_isPlaying())
                    co_yield nullptr;
                startTime = activeAudioSource->get_time();
            }
            
            PlayVideo(startTime);
        }
    }

    void PlaybackController::OnMenuSceneLoaded()
    {
        INFO("MenuSceneLoaded");
        StopAllCoroutines();
        previewWaitingForPreviewPlayer = true;
        get_gameObject()->SetActive(true);
        videoPlayer->Pause();
        videoPlayer->get_gameObject()->SetActive(false);
    }

    void PlaybackController::OnMenuSceneLoadedFresh(GlobalNamespace::ScenesTransitionSetupDataSO* transitionSetupData)
    {
        INFO("MenuSceneLoadedFresh");
        OnMenuSceneLoaded();
    }

    void PlaybackController::FrameReady(VideoPlayer*, int64_t frame)
    {
        if (!activeAudioSource || !activeAudioSource->m_CachedPtr || !videoConfig.has_value())
            return;
    }

    void PlaybackController::OnPrepareComplete(VideoPlayer*)
    {
        if(offsetAfterPrepare > 0)
        {
            std::chrono::duration<float> timeTaken = std::chrono::system_clock::now() - audioSourceStartTime;
            DEBUG("Prepare took {} seconds", timeTaken.count());
            float offset = timeTaken.count() + offsetAfterPrepare;
            videoPlayer->time =offset;
        }

        offsetAfterPrepare = 0;
        videoPlayer->Play();
    }

    void PlaybackController::PlayVideo(float startTime)
    {
        float totalOffset = videoConfig->get_offsetInSeconds();
        float songSpeed = 1;

        auto data = LevelData::get_currentGameplayCoreData();
        if(data)
        {
            songSpeed = data->gameplayModifiers->get_songSpeedMul();
            if(totalOffset + startTime < 0)
                totalOffset /= songSpeed * videoConfig->playbackSpeed;
        }

        videoPlayer->get_gameObject()->SetActive(true);
        videoPlayer->set_playbackSpeed(songSpeed * videoConfig->playbackSpeed);
        totalOffset += startTime;
        
        if(songSpeed * videoConfig->playbackSpeed < 1 && totalOffset > 0)
        {
            WARN("Disabling video player to prevent crashing");
            videoPlayer->Pause();
            videoPlayer->get_gameObject()->SetActive(false);
        }

        totalOffset += 0.0667f;

        if(videoConfig->endVideoAt.value_or(0) > 0)
            totalOffset = videoConfig->endVideoAt.value();

        // if(videoConfig.duration > 0)
        //     totalOffset = totalOffset % (float)videoConfig.duration;
        
        if(std::abs(totalOffset) < 0.001f)
        {
            totalOffset = 0;
        }

        StopAllCoroutines();

        if(activeAudioSource && activeAudioSource->get_time() > 0)
            lastKnownAudioSourceTime = activeAudioSource->get_time();

        DEBUG("Playing video with offset of {} seconds", totalOffset);
        if(totalOffset < 0)
        {
            StartCoroutine(custom_types::Helpers::CoroutineHelper::New(PlayVideoDelayedCoroutine(-totalOffset)));
        }
        else
        {
            videoPlayer->Play();
            if(!videoPlayer->get_isPrepared())
            {
                offsetAfterPrepare = totalOffset;
                audioSourceStartTime = std::chrono::system_clock::now();
            }
            else
                videoPlayer->set_time(totalOffset);
        }
    }

    custom_types::Helpers::Coroutine PlaybackController::PlayVideoDelayedCoroutine(float delayStartTime)
    {
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(delayStartTime));

        if (activeAudioSource && activeAudioSource->get_time() > 0)
        {
            lastKnownAudioSourceTime = activeAudioSource->get_time();
        }

        videoPlayer->Play();
    }

    void PlaybackController::SetSelectedLevel(GlobalNamespace::IPreviewBeatmapLevel* level, VideoConfig config)
    {
        previewWaitingForPreviewPlayer = true;
        previewWaitingForVideoPlayer = true;

        currentLevel = level;
        videoConfig = config;

        DEBUG("Selected level: {}", level->get_levelID());
        PrepareVideo(config);
    }

    void PlaybackController::PrepareVideo(VideoConfig video)
    {
        DEBUG("Preparing video");
        previewWaitingForVideoPlayer = true;

        if(prepareVideoCoroutine)
            StopCoroutine(prepareVideoCoroutine);

        prepareVideoCoroutine = custom_types::Helpers::CoroutineHelper::New(PrepareVideoCoroutine(video));
        videoPlayer->get_gameObject()->SetActive(true);
        StartCoroutine(prepareVideoCoroutine);
    }

    custom_types::Helpers::Coroutine PlaybackController::PrepareVideoCoroutine(VideoConfig video)
    {
        videoConfig = video;

        videoPlayer->url = video.get_videoPath();
        videoPlayer->Prepare();
        co_return;
    }
}