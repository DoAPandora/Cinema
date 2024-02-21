#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Video/VideoPlayer.hpp"
#include "System/MulticastDelegate.hpp"
#include "UnityEngine/AudioSource.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/Transform.hpp"

#include "DelegateW.hpp"

DECLARE_CLASS_CODEGEN(Cinema, CustomVideoPlayer, UnityEngine::MonoBehaviour,

    DECLARE_INSTANCE_FIELD(UnityEngine::Video::VideoPlayer*, player);
    DECLARE_INSTANCE_FIELD(UnityEngine::AudioSource*, videoPlayerAudioSource);

    DECLARE_INSTANCE_FIELD(UnityEngine::Renderer*, screenRenderer);
    DECLARE_INSTANCE_FIELD(UnityEngine::RenderTexture*, renderTexture);

    DECLARE_INSTANCE_FIELD(float, volumeScale);
    DECLARE_INSTANCE_FIELD(bool, muted);
    DECLARE_INSTANCE_FIELD(bool, bodyVisible);
    DECLARE_INSTANCE_FIELD(bool, waitingForFadeOut);

    UnityEngine::Video::VideoPlayer::ErrorEventHandler* videoPlayerErrorReceived;
    UnityEngine::Video::VideoPlayer::EventHandler* videoPlayerPrepareComplete;
    UnityEngine::Video::VideoPlayer::EventHandler* videoPlayerStarted;
    UnityEngine::Video::VideoPlayer::EventHandler* videoPlayerFinished;

    DECLARE_DEFAULT_CTOR();

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);
    DECLARE_INSTANCE_METHOD(void, CreateScreen);
    DECLARE_INSTANCE_METHOD(void, FadeControllerUpdate, float value);
    DECLARE_INSTANCE_METHOD(void, OnMenuSceneLoaded);
    DECLARE_INSTANCE_METHOD(void, FirstFrameReady, UnityEngine::Video::VideoPlayer*, int64_t frame);
    DECLARE_INSTANCE_METHOD(void, SetBrightness, float brightness);
    DECLARE_INSTANCE_METHOD(void, LoopVideo, bool loop);
    DECLARE_INSTANCE_METHOD(void, Show);
    DECLARE_INSTANCE_METHOD(void, FadeIn, float duration = 0.4f);
    DECLARE_INSTANCE_METHOD(void, Hide);
    DECLARE_INSTANCE_METHOD(void, FadeOut, float duration = 0.7f);
    DECLARE_INSTANCE_METHOD(void, ShowScreenBody);
    DECLARE_INSTANCE_METHOD(void, HideScreenBody);
    DECLARE_INSTANCE_METHOD(void, Play);
    DECLARE_INSTANCE_METHOD(void, Pause);
    DECLARE_INSTANCE_METHOD(void, Stop);
    DECLARE_INSTANCE_METHOD(void, Prepare);
    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_METHOD(void, UpdateScreenContent);
    DECLARE_INSTANCE_METHOD(void, SetTexture, UnityEngine::Texture* texture);
    DECLARE_INSTANCE_METHOD(void, SetCoverTexture, UnityEngine::Texture* texture);
    DECLARE_INSTANCE_METHOD(void, SetStaticTexture, UnityEngine::Texture* texture);
    DECLARE_INSTANCE_METHOD(void, ClearTexture);

    DECLARE_INSTANCE_METHOD(UnityEngine::Video::VideoPlayer*, CreateVideoPlayer, UnityEngine::Transform* parent);

    DECLARE_INSTANCE_METHOD(void, VideoPlayerPrepareComplete, UnityEngine::Video::VideoPlayer* source);
    DECLARE_INSTANCE_METHOD(void, VideoPlayerStarted, UnityEngine::Video::VideoPlayer* source);
    DECLARE_INSTANCE_METHOD(void, VideoPlayerFinished, UnityEngine::Video::VideoPlayer* source);
    DECLARE_INSTANCE_METHOD(void, VideoPlayerErrorReceived, UnityEngine::Video::VideoPlayer* source, StringW message);

    DECLARE_INSTANCE_METHOD(float, GetVideoAspectRatio);
    DECLARE_INSTANCE_METHOD(void, Mute);
    DECLARE_INSTANCE_METHOD(void, Unmute);
    DECLARE_INSTANCE_METHOD(void, SetSoftParent, UnityEngine::Transform* parent);


    DECLARE_INSTANCE_METHOD(UnityEngine::Color, get_ScreenColor);
    DECLARE_INSTANCE_METHOD(void, set_ScreenColor, UnityEngine::Color value);
    DECLARE_INSTANCE_METHOD(float, get_PlaybackSpeed);
    DECLARE_INSTANCE_METHOD(void, set_PlaybackSpeed, float value);
    DECLARE_INSTANCE_METHOD(float, get_VideoDuration);
    DECLARE_INSTANCE_METHOD(void, set_Volume, float value);
    DECLARE_INSTANCE_METHOD(void, set_PanStereo, float value);
    DECLARE_INSTANCE_METHOD(StringW, get_Url);
    DECLARE_INSTANCE_METHOD(void, set_Url, StringW value);
    DECLARE_INSTANCE_METHOD(bool, get_IsPlaying);
    DECLARE_INSTANCE_METHOD(bool, get_IsFading);
    DECLARE_INSTANCE_METHOD(bool, get_IsPrepared);
    DECLARE_INSTANCE_METHOD(void, set_time, double value);
    DECLARE_INSTANCE_METHOD(void, set_sendFrameReadyEvents, bool value);

    public:

    void SetDefaultMenuPlacement(std::optional<float> width = std::nullopt);
    // void SetPlacement

    static UnityEngine::Shader* GetShader();
    void SetBloomIntensity(std::optional<float> bloomIntensity);

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
)