#include "Screen/PlaybackController.hpp"
#include "Hooks/LevelData.hpp"
#include "Util/Events.hpp"
#include "Video/VideoLoader.hpp"
#include "main.hpp"

#include "bs-events/shared/BSEvents.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/WaitForSeconds.hpp"

#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/SceneInfo.hpp"

#include "BeatSaber/GameSettings/Audio.hpp"
#include "BeatSaber/GameSettings/MainSettings.hpp"

#include "bsml/shared/Helpers/getters.hpp"

DEFINE_TYPE(Cinema, PlaybackController);

using namespace UnityEngine;
using namespace Video;

namespace Cinema
{

    PlaybackController* PlaybackController::get_instance()
    {
        return instance;
    }

    void PlaybackController::Destroy() {}

    void PlaybackController::Create()
    {
        if(instance)
        {
            return;
        }
        GameObject::New_ctor("CinemaPlaybackController")->AddComponent<PlaybackController*>();
    }

    void PlaybackController::Start()
    {
        if(instance)
        {
            Object::Destroy(this);
            return;
        }
        instance = this;
        INFO("Created PlaybackController");
        GameObject::DontDestroyOnLoad(gameObject);
        videoPlayer = gameObject->AddComponent<CustomVideoPlayer*>();
        //         videoPlayer->set_sendFrameReadyEvents(true);

        onPrepareComplete = custom_types::MakeDelegate<VideoPlayer::EventHandler*>(
            std::function<void(VideoPlayer*)>(
                std::bind_front(&PlaybackController::OnPrepareComplete, this)));
        videoPlayer->player->prepareCompleted = (VideoPlayer::EventHandler*)System::Delegate::Combine(videoPlayer->player->prepareCompleted, onPrepareComplete);

        //         videoPlayer->player->prepareCompleted += onPrepareComplete;
        // //        fr = {&PlaybackController::FrameReady, this};
        // //        videoPlayer->player->frameReady += frameReady;

        BSEvents::songPaused += {&PlaybackController::PauseVideo, this};
        BSEvents::songUnpaused += {&PlaybackController::ResumeVideo, this};
        BSEvents::menuSceneLoaded += {&PlaybackController::OnMenuSceneLoaded, this};
        BSEvents::lateMenuSceneLoadedFresh += {&PlaybackController::OnMenuSceneLoadedFresh, this};
        BSEvents::gameSceneActive += {&PlaybackController::GameSceneActive, this};
        BSEvents::gameSceneLoaded += {&PlaybackController::GameSceneLoaded, this};

        // OnMenuSceneLoadedFresh(nullptr);
    }

    void PlaybackController::OnDestroy() {}

    void PlaybackController::PauseVideo()
    {
        StopAllCoroutines();
        if(videoPlayer->IsPrepared)
        {
            videoPlayer->Pause();
        }
    }

    void PlaybackController::ResumeVideo()
    {
        if(!getModConfig().enabled.GetValue() || videoPlayer->IsPlaying)
            return;

        float referenceTime = GetReferenceTime();
        if(referenceTime > 0)
        {
            videoPlayer->Play();
        }
        else
            StartCoroutine(prepareVideoCoroutine);
    }

    void PlaybackController::ApplyOffset(int offset) {}

    float PlaybackController::GetReferenceTime(std::optional<float> referenceTime, std::optional<float> playbackSpeed)
    {
        if(!activeAudioSource)
            return 0;

        float time = 0;
        if(referenceTime == 0 && activeAudioSource->get_time() == 0)
            time = lastKnownAudioSourceTime;
        else
            time == referenceTime == 0 ? activeAudioSource->get_time() : referenceTime;

        float speed = *playbackSpeed == 0 ? *videoConfig->playbackSpeed : *playbackSpeed;

        return time * speed + (float)videoConfig->offset / 1000;
    }

    void PlaybackController::PlayerStartedAfterResync(UnityEngine::Video::VideoPlayer*, int64_t frame) {}

    void PlaybackController::FrameReady(UnityEngine::Video::VideoPlayer*, int64_t frame) {}

    void PlaybackController::Update() {}

    void PlaybackController::StartPreview() {}

    void PlaybackController::StopPreview(bool stopPreviewMusic) {}

    void PlaybackController::OnMenuSceneLoaded()
    {
        INFO("MenuSceneLoaded");
        StopAllCoroutines();
        previewWaitingForPreviewPlayer = true;
        get_gameObject()->SetActive(true);
        videoPlayer->Pause();
    }

    void PlaybackController::OnMenuSceneLoadedFresh(GlobalNamespace::ScenesTransitionSetupDataSO* transitionSetupData)
    {
        OnMenuSceneLoaded();

        mainSettingsHandler = BSML::Helpers::GetDiContainer()->Resolve<BeatSaber::GameSettings::MainSettingsHandler*>();
        if(mainSettingsHandler)
        {
            this->videoPlayer->volumeScale = mainSettingsHandler->get_instance()->audioSettings->volume;
        }
    }

    void PlaybackController::SceneChanged() {}

    void PlaybackController::OnConfigChanged(std::shared_ptr<VideoConfig> config) {}

    void PlaybackController::OnConfigChanged(std::shared_ptr<VideoConfig> config, bool reloadVideo) {}

    void PlaybackController::ConfigChangedPrepareHandler(UnityEngine::Video::VideoPlayer* sender) {}

    void PlaybackController::ConfigChangedFrameReadyHandler(UnityEngine::Video::VideoPlayer* sender, int64_t frameIdx) {}

    void PlaybackController::ShowSongCover() {}

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

        auto level = data->beatmapLevel;
        auto config = VideoLoader::GetConfigForLevel(level);
        if(config != nullptr)
        {
            videoPlayer->get_gameObject()->SetActive(false);
            INFO("Could not find config for level");
            return;
        }

        SetSelectedLevel(level, config);
        if(!config->get_isPlayable())
            return;

        StartCoroutine(custom_types::Helpers::CoroutineHelper::New(PlayVideoAfterAudioSourceCoroutine(false)));
    }

    void PlaybackController::SetAudioSourcePanning(float pan) {}

    void PlaybackController::PlayVideo(float startTime)
    {
        float totalOffset = videoConfig->GetOffsetInSec();
        float songSpeed = 1;

        auto data = LevelData::get_currentGameplayCoreData();
        if(data)
        {
            songSpeed = data->gameplayModifiers->get_songSpeedMul();
            if(totalOffset + startTime < 0)
                totalOffset /= songSpeed * *videoConfig->playbackSpeed;
        }

        videoPlayer->get_gameObject()->SetActive(true);
        videoPlayer->PlaybackSpeed = songSpeed * *videoConfig->playbackSpeed;
        totalOffset += startTime;

        if(songSpeed * *videoConfig->playbackSpeed < 1 && totalOffset > 0)
        {
            WARN("Disabling video player to prevent crashing");
            videoPlayer->Pause();
            videoPlayer->get_gameObject()->SetActive(false);
        }

        totalOffset += 0.0667f;

        if(videoConfig->endVideoAt.value_or(0) > 0)
            totalOffset = videoConfig->endVideoAt.value();

        if(videoConfig->duration > 0)
            totalOffset = fmod(totalOffset, (float)videoConfig->duration);

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
            if(!videoPlayer->IsPrepared)
            {
                offsetAfterPrepare = totalOffset;
                audioSourceStartTime = std::chrono::system_clock::now();
            }
            else
                videoPlayer->set_time(totalOffset);
        }
    }

    void PlaybackController::OnPrepareComplete(VideoPlayer*)
    {
        if(offsetAfterPrepare > 0)
        {
            std::chrono::duration<float> timeTaken = std::chrono::system_clock::now() - audioSourceStartTime;
            DEBUG("Prepare took {} seconds", timeTaken.count());
            float offset = timeTaken.count() + offsetAfterPrepare;
            videoPlayer->time = offset;
        }

        offsetAfterPrepare = 0;
        videoPlayer->Play();
    }

    void PlaybackController::StopPlayback() {}

    void PlaybackController::SceneTransitionInitCalled() {}

    void PlaybackController::UpdateSongPreviewPlayer(UnityEngine::AudioSource* activeAudioSource, float startTime, float timeRemaining, bool isDefault) {}

    void PlaybackController::StartSongPreview() {}

    custom_types::Helpers::Coroutine PlaybackController::PlayVideoAfterAudioSourceCoroutine(bool preview)
    {
        float startTime;
        DEBUG("PlayAfterAudioSource");

        if(!preview)
        {
            GlobalNamespace::AudioTimeSyncController* timeSyncController;
            while(timeSyncController = Resources::FindObjectsOfTypeAll<GlobalNamespace::AudioTimeSyncController*>().front_or_default(), !timeSyncController)
            {
                co_yield nullptr;
            }

            audioTimeSyncController = Resources::FindObjectsOfTypeAll<GlobalNamespace::AudioTimeSyncController*>().front_or_default([](GlobalNamespace::AudioTimeSyncController* x)
                                                                                                                                    { return x->get_transform()->get_parent()->get_parent()->get_name()->Contains("StandardGameplay"); });
            activeAudioSource = audioTimeSyncController->_audioSource;

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

    custom_types::Helpers::Coroutine PlaybackController::PlayVideoDelayedCoroutine(float delayStartTime)
    {
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(WaitForSeconds::New_ctor(delayStartTime));

        if(activeAudioSource && activeAudioSource->get_time() > 0)
        {
            lastKnownAudioSourceTime = activeAudioSource->get_time();
        }

        videoPlayer->Play();
    }

    void PlaybackController::SetSelectedLevel(GlobalNamespace::BeatmapLevel* level, std::shared_ptr<VideoConfig> config)
    {
        previewWaitingForPreviewPlayer = true;
        previewWaitingForVideoPlayer = true;

        currentLevel = level;
        videoConfig = config;

        DEBUG("Selected level: {}", level->levelID);

        if(config == nullptr)
        {
            videoPlayer->FadeOut();
            StopAllCoroutines();
            return;
        }

        DEBUG("Preparing video");
        PrepareVideo(config);
        if(level && VideoLoader::IsDlcSong(level))
        {
            videoPlayer->FadeOut();
        }
    }

    void PlaybackController::PrepareVideo(std::shared_ptr<VideoConfig> video)
    {
        if(!video)
        {
            return ERROR("VideoConfig was null!");
        }
        DEBUG("Preparing video");
        previewWaitingForVideoPlayer = true;

        if(prepareVideoCoroutine)
            StopCoroutine(prepareVideoCoroutine);

        prepareVideoCoroutine = custom_types::Helpers::CoroutineHelper::New(PrepareVideoCoroutine(video));
        videoPlayer->get_gameObject()->SetActive(true);
        StartCoroutine(prepareVideoCoroutine);
    }

    custom_types::Helpers::Coroutine PlaybackController::PrepareVideoCoroutine(std::shared_ptr<VideoConfig> video)
    {
        videoConfig = video;

        videoPlayer->Pause();
        if(videoConfig->downloadState != DownloadState::Downloaded)
        {
            DEBUG("Video is not downloaded!");
            videoPlayer->FadeOut();
            co_return;
        }

        videoPlayer->LoopVideo(video->loop.value_or(false));

        if(!video->get_VideoPath())
        {
            ERROR("Video path was null!");
            co_return;
        }

        std::string videoPath = video->get_VideoPath().value();
        INFO("Loading video {}", videoPath);

        videoPlayer->Url = videoPath;
        videoPlayer->Prepare();
        
        co_return;
    }
} // namespace Cinema