#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "Screen/CustomVideoPlayer.hpp"
#include "Video/VideoConfig.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/AudioSource.hpp"
#include "UnityEngine/Video/VideoPlayer_EventHandler.hpp"
#include "UnityEngine/Video/VideoPlayer_FrameReadyEventHandler.hpp"

#include "GlobalNamespace/SongPreviewPlayer.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "GlobalNamespace/ScenesTransitionSetupDataSO.hpp"

DECLARE_CLASS_CODEGEN(Cinema, PlaybackController, UnityEngine::MonoBehaviour,

    DECLARE_INSTANCE_FIELD(CustomVideoPlayer*, videoPlayer);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::AudioTimeSyncController*, audioTimeSyncController);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::SongPreviewPlayer*, songPreviewPlayer);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::IPreviewBeatmapLevel*, currentLevel);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::MainSettingsModelSO*, mainSettingsModel);
    DECLARE_INSTANCE_FIELD(UnityEngine::AudioSource*, activeAudioSource);
    DECLARE_INSTANCE_FIELD(System::Collections::IEnumerator*, prepareVideoCoroutine);

    DECLARE_INSTANCE_FIELD(float, lastKnownAudioSourceTime);
    DECLARE_INSTANCE_FIELD(float, previewStartTime);
    DECLARE_INSTANCE_FIELD(float, previewTimeRemaining);
    DECLARE_INSTANCE_FIELD_DEFAULT(bool, previewWaitingForVideoPlayer, true);
    DECLARE_INSTANCE_FIELD(bool, previewWaitingForPreviewPlayer);
    DECLARE_INSTANCE_FIELD(float, offsetAfterPrepare);

    public:

    std::optional<VideoConfig> videoConfig;
    std::chrono::system_clock::time_point audioSourceStartTime;

    DelegateW<UnityEngine::Video::VideoPlayer::EventHandler> prepareCompleted;
    DelegateW<UnityEngine::Video::VideoPlayer::FrameReadyEventHandler> frameReady;

    private:

    static PlaybackController* instance;

    DECLARE_INSTANCE_METHOD(void, Start);
    DECLARE_INSTANCE_METHOD(void, Update);

    DECLARE_INSTANCE_METHOD(void, PlayVideo, float startTime);
    DECLARE_INSTANCE_METHOD(void, ResumeVideo);
    DECLARE_INSTANCE_METHOD(void, PauseVideo);
    DECLARE_INSTANCE_METHOD(float, GetReferenceTime, float referenceTime = 0, float playbackSpeed = 0);

    DECLARE_INSTANCE_METHOD(void, FrameReady, UnityEngine::Video::VideoPlayer*, int64_t frame);
    DECLARE_INSTANCE_METHOD(void, OnPrepareComplete, UnityEngine::Video::VideoPlayer*);

    DECLARE_INSTANCE_METHOD(void, GameSceneActive);
    DECLARE_INSTANCE_METHOD(void, GameSceneLoaded);
    DECLARE_INSTANCE_METHOD(void, OnMenuSceneLoaded);
    DECLARE_INSTANCE_METHOD(void, OnMenuSceneLoadedFresh, GlobalNamespace::ScenesTransitionSetupDataSO* transitionSetupData);

    public:

    static PlaybackController* get_instance();
    __declspec(property(get=get_instance)) PlaybackController* Instance;

    static void Create();
    void SetSelectedLevel(GlobalNamespace::IPreviewBeatmapLevel* level, VideoConfig config);
    void PrepareVideo(VideoConfig video);
    custom_types::Helpers::Coroutine PlayVideoAfterAudioSourceCoroutine(bool preview);
    custom_types::Helpers::Coroutine PrepareVideoCoroutine(VideoConfig video);
    custom_types::Helpers::Coroutine PlayVideoDelayedCoroutine(float delayStartTime);

)