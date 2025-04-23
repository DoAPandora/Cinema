#pragma once

#include "custom-types/shared/macros.hpp"

#include "sombrero/shared/FastColor.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Video/VideoPlayer.hpp"
#include "UnityEngine/AudioSource.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/Transform.hpp"

#include "ScreenController.hpp"
#include "Placement.hpp"
#include "Util/EasingController.hpp"

DECLARE_CLASS_CODEGEN(Cinema, CustomVideoPlayer, UnityEngine::MonoBehaviour) {

    DECLARE_INSTANCE_FIELD(UnityEngine::Video::VideoPlayer*, player);
    DECLARE_INSTANCE_FIELD(UnityEngine::AudioSource*, videoPlayerAudioSource);
    DECLARE_INSTANCE_FIELD(ScreenController*, screenController);
    DECLARE_INSTANCE_FIELD(UnityEngine::Renderer*, screenRenderer);
    DECLARE_INSTANCE_FIELD(UnityEngine::RenderTexture*, renderTexture);

    inline static constexpr float MAX_VOLUME = 0.28f;
    float volumeScale = 1;
    bool muted = true;
    bool bodyVisible;
    bool waitingForFadeOut;
    bool videoEnded;
    
    EasingController fadeController;

public:
    inline static ConstString MAIN_TEXTURE_NAME = "_MainTex";
    inline static ConstString CINEMA_TEXTURE_NAME ="_CinemaVideoTexture";
    inline static ConstString STATUS_PROPERTY_NAME = "_CinemaVideoIsPlaying";
    inline static constexpr float MAX_BRIGHTNESS = 0.92f;
    static constexpr Sombrero::FastColor screenColorOn = Sombrero::FastColor::white().Alpha(0) * MAX_BRIGHTNESS;
    static constexpr Sombrero::FastColor screenColorOff = Sombrero::FastColor::clear();
    inline static int MainTex = UnityEngine::Shader::PropertyToID(MAIN_TEXTURE_NAME);
    inline static int CinemaVideoTexture = UnityEngine::Shader::PropertyToID(CINEMA_TEXTURE_NAME);
    inline static int CinemaStatusProperty = UnityEngine::Shader::PropertyToID(STATUS_PROPERTY_NAME);

    std::string currentlyPlayingVideo;
    UnityEngine::Video::VideoPlayer::ErrorEventHandler* videoPlayerErrorReceived;
    UnityEngine::Video::VideoPlayer::EventHandler* videoPlayerPrepareComplete;
    UnityEngine::Video::VideoPlayer::EventHandler* videoPlayerStarted;
    UnityEngine::Video::VideoPlayer::EventHandler* videoPlayerFinished;
    UnityEngine::Video::VideoPlayer::FrameReadyEventHandler* videoPlayerFirstFrameReady;

    DECLARE_DEFAULT_CTOR();

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);
    DECLARE_INSTANCE_METHOD(void, Update);

public:

    void CreateScreen();
    void FadeControllerUpdate(float value);
    void OnMenuSceneLoaded();
    void FirstFrameReady(UnityEngine::Video::VideoPlayer*, int64_t frame);
    void SetBrightness(float brightness);
    void LoopVideo(bool loop);
    void Show();
    void FadeIn(float duration = 0.4f);
    void Hide();
    void FadeOut(float duration = 0.7f);
    void ShowScreenBody();
    void HideScreenBody();
    void Play();
    void Pause();
    void Stop();
    void Prepare();
    void UpdateScreenContent();
    void SetTexture(UnityEngine::Texture* texture);
    void SetCoverTexture(UnityEngine::Texture* texture);
    void SetStaticTexture(UnityEngine::Texture* texture);
    void ClearTexture();

    void VideoPlayerPrepareComplete(UnityEngine::Video::VideoPlayer* source);
    void VideoPlayerStarted(UnityEngine::Video::VideoPlayer* source);
    void VideoPlayerFinished(UnityEngine::Video::VideoPlayer* source);
    void VideoPlayerErrorReceived(UnityEngine::Video::VideoPlayer* source, StringW message);

    float GetVideoAspectRatio();
    void Mute();
    void Unmute();
    void SetSoftParent(UnityEngine::Transform* parent);

    void SetDefaultMenuPlacement(std::optional<float> width = std::nullopt);
    void SetPlacement(const Placement& placement);

    static UnityEngine::Shader* GetShader();
    void SetBloomIntensity(std::optional<float> bloomIntensity);

    UnityEngine::Color get_ScreenColor();
    void set_ScreenColor(UnityEngine::Color value);
    float get_PlaybackSpeed();
    void set_PlaybackSpeed(float value);
    float get_VideoDuration();
    void set_Volume(float value);
    void set_PanStereo(float value);
    StringW get_Url();
    void set_Url(StringW value);
    bool get_IsPlaying();
    bool get_IsFading();
    bool get_IsPrepared();
    void set_time(double value);
    void set_sendFrameReadyEvents(bool value);

    bool get_VideoEnded();

    __declspec(property(get=get_ScreenColor, put=set_ScreenColor)) UnityEngine::Color ScreenColor;
    __declspec(property(get=get_PlaybackSpeed, put=set_PlaybackSpeed)) float PlaybackSpeed;
    __declspec(property(get=get_VideoDuration)) float VideoDuration;
    __declspec(property(put=set_Volume)) float Volume;
    __declspec(property(put=set_PanStereo)) float PanStereo;
    __declspec(property(get=get_Url, put=set_Url)) StringW Url;
    __declspec(property(get=get_IsPlaying)) bool IsPlaying;
    __declspec(property(get=get_IsFading)) bool IsFading;
    __declspec(property(get=get_IsPrepared)) bool IsPrepared;
    __declspec(property(put=set_time)) double time;
    __declspec(property(put=set_sendFrameReadyEvents)) bool sendFrameReadyEvents;
};