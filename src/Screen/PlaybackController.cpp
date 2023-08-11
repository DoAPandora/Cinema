#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-experimental-coroutine"

#include "main.hpp"
#include "Screen/PlaybackController.hpp"
#include "Hooks/LevelData.hpp"
#include "Video/VideoLoader.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/WaitForSeconds.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"

#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/SceneInfo.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"

DEFINE_TYPE(Cinema, PlaybackController);

namespace Cinema {
    
    PlaybackController* PlaybackController::instance = nullptr;
    PlaybackController* PlaybackController::get_instance() { return instance; }
    void PlaybackController::Create()
    {
        if(instance && instance->m_CachedPtr.m_value)
            return;
        UnityEngine::GameObject::New_ctor("CinemaPlaybackController")->AddComponent<PlaybackController*>();
    }

    void PlaybackController::Start()
    {
        if(instance && instance->m_CachedPtr.m_value)
        {
            UnityEngine::Object::Destroy(this);
            return;
        }
        instance = this;
        getLogger().info("Created PlaybackController");

        UnityEngine::GameObject::DontDestroyOnLoad(this->get_gameObject());
        videoPlayer = VideoPlayer::CreateVideoPlayer();
        videoPlayer->get_gameObject()->SetActive(false);
        videoPlayer->set_sendFrameReadyEvents(true);

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

        float speed = playbackSpeed == 0 ? videoConfig.playbackSpeed : playbackSpeed;
        
        return time * speed + videoConfig.offset / 1000;
    }

    void PlaybackController::GameSceneActive()
    {
        auto data = LevelData::get_currentGameplayCoreData();
        if(!data)
            return;

        StringW sceneName = data->environmentInfo->sceneInfo->sceneName;
        auto scene = UnityEngine::SceneManagement::SceneManager::GetSceneByName(sceneName);
		/* Crashes on next scene transition */
        // if(scene.IsValid())
        //     UnityEngine::SceneManagement::SceneManager::MoveGameObjectToScene(get_gameObject(), scene);
        getLogger().info("Moving to game scene");
    }

    void PlaybackController::GameSceneLoaded()
    {
        get_gameObject()->SetActive(true);
        getLogger().info("GameSceneLoaded");
        StopAllCoroutines();

        if(!getModConfig().enabled.GetValue())
        {
            getLogger().info("Mod disabled");
            return;
        }

        videoPlayer->get_gameObject()->SetActive(false);

        auto data = LevelData::get_currentGameplayCoreData();
        if(!data)
        {
            getLogger().info("LevelData was null");
            return;
        }

        auto level = data->previewBeatmapLevel;
        auto config = VideoLoader::GetConfigForLevel(level);
        if(!config.has_value())
        {
            videoPlayer->get_gameObject()->SetActive(false);
            getLogger().info("Could not find config for level");
            return;
        }

        SetSelectedLevel(level, config.value());
        if(!config->get_isPlayable())
            return;

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(PlayVideoAfterAudioSourceCoroutine(false)));
    }

    Coroutine PlaybackController::PlayVideoAfterAudioSourceCoroutine(bool preview)
    {
        float startTime;

        if(!preview)
        {
            GlobalNamespace::AudioTimeSyncController* timeSyncController;
            while(!timeSyncController)
            {
                timeSyncController = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::AudioTimeSyncController*>().FirstOrDefault();
                co_yield nullptr;
            }

            audioTimeSyncController = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::AudioTimeSyncController*>().FirstOrDefault([](GlobalNamespace::AudioTimeSyncController* x) { return x->get_transform()->get_parent()->get_parent()->get_name()->Contains("StandardGameplay"); });
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
        getLogger().info("MenuSceneLoaded");
        StopAllCoroutines();
        previewWaitingForPreviewPlayer = true;
        get_gameObject()->SetActive(true);
        videoPlayer->Pause();
        videoPlayer->get_gameObject()->SetActive(false);
    }

    void PlaybackController::OnMenuSceneLoadedFresh(GlobalNamespace::ScenesTransitionSetupDataSO* transitionSetupData)
    {
        getLogger().info("MenuSceneLoadedFresh");
        OnMenuSceneLoaded();
    }

    void PlaybackController::FrameReady(long frame) {}

    void PlaybackController::OnPrepareComplete()
    {
        if(offsetAfterPrepare > 0)
        {
            std::chrono::duration<float> timeTaken = std::chrono::system_clock::now() - audioSourceStartTime;
            getLogger().info("Prepare took %f seconds", timeTaken.count());
            float offset = timeTaken.count() + offsetAfterPrepare;
            videoPlayer->set_time(offset);
        }
        offsetAfterPrepare = 0;
        videoPlayer->Play();
    }

    void PlaybackController::PlayVideo(float startTime)
    {
        float totalOffset = videoConfig.get_offsetInSeconds();
        float songSpeed = 1;

        auto data = LevelData::get_currentGameplayCoreData();
        if(data)
        {
            songSpeed = data->gameplayModifiers->get_songSpeedMul();
            if(totalOffset + startTime < 0)
                totalOffset /= songSpeed * videoConfig.playbackSpeed;
        }

        videoPlayer->get_gameObject()->SetActive(true);
        videoPlayer->set_playbackSpeed(songSpeed * videoConfig.playbackSpeed);
        totalOffset += startTime;
        
        if(songSpeed * videoConfig.playbackSpeed < 1 && totalOffset > 0)
        {
            getLogger().warning("Disabling video player to prevent crashing");
            videoPlayer->Pause();
            videoPlayer->get_gameObject()->SetActive(false);
        }

        totalOffset += 0.0667f;

        if(videoConfig.endVideoAt.value_or(0) > 0)
            totalOffset = videoConfig.endVideoAt.value();

        // if(videoConfig.duration > 0)
        //     totalOffset = totalOffset % (float)videoConfig.duration;
        
        if(std::abs(totalOffset) < 0.001f)
        {
            totalOffset = 0;
        }

        StopAllCoroutines();

        if(activeAudioSource && activeAudioSource->get_time() > 0)
            lastKnownAudioSourceTime = activeAudioSource->get_time();

        getLogger().info("Playing video with offset of %f seconds", totalOffset);
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
    
    Coroutine PlaybackController::PlayVideoDelayedCoroutine(float delayStartTime)
    {
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForSeconds::New_ctor(delayStartTime));

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

        getLogger().info("Selected level: %s", static_cast<std::string>(level->get_levelID()).c_str());
        PrepareVideo(config);
    }

    void PlaybackController::PrepareVideo(VideoConfig video)
    {
        previewWaitingForVideoPlayer = true;

        if(prepareVideoCoroutine)
            StopCoroutine(prepareVideoCoroutine);

        prepareVideoCoroutine = custom_types::Helpers::CoroutineHelper::New(PrepareVideoCoroutine(video));
        videoPlayer->get_gameObject()->SetActive(true);
        StartCoroutine(prepareVideoCoroutine);
    }

    Coroutine PlaybackController::PrepareVideoCoroutine(VideoConfig video)
    {
        videoConfig = video;

        videoPlayer->set_url(video.get_videoPath());
        videoPlayer->Prepare();
        co_return;
    }
}
#pragma clang diagnostic pop