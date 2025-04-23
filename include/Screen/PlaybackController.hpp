#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "Screen/CustomVideoPlayer.hpp"
#include "Video/VideoConfig.hpp"
#include "Util/EventArgs.hpp"
#include "Util/Scene.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/AudioSource.hpp"

#include "GlobalNamespace/SongPreviewPlayer.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"
#include "GlobalNamespace/ScenesTransitionSetupDataSO.hpp"

DECLARE_CLASS_CODEGEN(Cinema, PlaybackController, UnityEngine::MonoBehaviour) {

    DECLARE_INSTANCE_FIELD(CustomVideoPlayer*, videoPlayer);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::AudioTimeSyncController*, audioTimeSyncController);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::SongPreviewPlayer*, songPreviewPlayer);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::BeatmapLevel*, currentLevel);
    DECLARE_INSTANCE_FIELD(UnityEngine::AudioSource*, activeAudioSource);
    DECLARE_INSTANCE_FIELD(System::Collections::IEnumerator*, prepareVideoCoroutine);

    float lastKnownAudioSourceTime;
    float previewStartTime;
    float previewTimeRemaining;
    bool previewWaitingForVideoPlayer = true;
    bool previewWaitingForPreviewPlayer;
    float offsetAfterPrepare;
    static inline UnityW<PlaybackController> instance;

public:

    Scene activeScene;
    std::shared_ptr<VideoConfig> videoConfig;
    std::chrono::system_clock::time_point audioSourceStartTime;

    UnityEngine::Video::VideoPlayer::EventHandler* configChangedPrepareHandler;
    UnityEngine::Video::VideoPlayer::FrameReadyEventHandler* configChangedFrameReadyHandler;
    UnityEngine::Video::VideoPlayer::EventHandler* onPrepareComplete;

    DECLARE_INSTANCE_METHOD(void, Destroy);
    DECLARE_INSTANCE_METHOD(void, Start);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);
    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_DEFAULT_CTOR();

public:

    void PauseVideo();
    void ResumeVideo();
    void ApplyOffset(int offset);
    void PlayerStartedAfterResync(UnityEngine::Video::VideoPlayer*, int64_t frame);
    void FrameReady(UnityEngine::Video::VideoPlayer*, int64_t frame);
    void StartPreview();
    void StopPreview(bool stopPreviewMusic);
    void OnMenuSceneLoaded();
    void OnMenuSceneLoadedFresh(GlobalNamespace::ScenesTransitionSetupDataSO* transitionSetupData);
    void SceneChanged();
    void ConfigChangedPrepareHandler(UnityEngine::Video::VideoPlayer* sender);
    void ConfigChangedFrameReadyHandler(UnityEngine::Video::VideoPlayer* sender, int64_t frameIdx);
    void ShowSongCover();
    void GameSceneActive();
    void GameSceneLoaded();
    void SetAudioSourcePanning(float pan);
    void PlayVideo(float startTime);
    void OnPrepareComplete(UnityEngine::Video::VideoPlayer*);
    void StopPlayback();
    void SceneTransitionInitCalled();
    void UpdateSongPreviewPlayer(UnityEngine::AudioSource* activeAudioSource, float startTime, float timeRemaining, bool isDefault);
    void StartSongPreview();

    static void Create();
    float GetReferenceTime(std::optional<float> referenceTime = std::nullopt, std::optional<float> playbackSpeed = std::nullopt);
    void ResyncVideo(std::optional<float> referenceTime = std::nullopt, std::optional<float> playbackSpeed = std::nullopt);
    void OnConfigChanged(std::shared_ptr<VideoConfig> config);
    void OnConfigChanged(std::shared_ptr<VideoConfig> config, bool reloadVideo);
    void SetSelectedLevel(GlobalNamespace::BeatmapLevel* level, std::shared_ptr<VideoConfig> config);
    void DifficultySelected(ExtraSongDataArgs extraSongDataArgs);
    
    void PrepareVideo(std::shared_ptr<VideoConfig> video);
    custom_types::Helpers::Coroutine PlayVideoAfterAudioSourceCoroutine(bool preview);
    custom_types::Helpers::Coroutine PlayVideoDelayedCoroutine(float delayStartTime);
    custom_types::Helpers::Coroutine PrepareVideoCoroutine(std::shared_ptr<VideoConfig> video);
    
        static PlaybackController* get_instance();
};