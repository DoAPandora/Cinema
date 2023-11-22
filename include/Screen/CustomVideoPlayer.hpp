#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Video/VideoPlayer.hpp"
#include "UnityEngine/AudioSource.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Video/VideoPlayer_EventHandler.hpp"
#include "UnityEngine/Video/VideoPlayer_ErrorEventHandler.hpp"

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

    DelegateW<UnityEngine::Video::VideoPlayer::ErrorEventHandler> videoPlayerErrorReceived;
    DelegateW<UnityEngine::Video::VideoPlayer::EventHandler> videoPlayerPrepareComplete;
    DelegateW<UnityEngine::Video::VideoPlayer::EventHandler> videoPlayerStarted;
    DelegateW<UnityEngine::Video::VideoPlayer::EventHandler> videoPlayerFinished;

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnDestroy);
    DECLARE_INSTANCE_METHOD(void, CreateScreen);
    DECLARE_INSTANCE_METHOD(void, Play);
    DECLARE_INSTANCE_METHOD(void, Pause);
    DECLARE_INSTANCE_METHOD(void, Prepare);
    DECLARE_INSTANCE_METHOD(UnityEngine::Video::VideoPlayer*, CreateVideoPlayer, UnityEngine::Transform* parent);

    DECLARE_INSTANCE_METHOD(void, VideoPlayerErrorReceived, UnityEngine::Video::VideoPlayer* source, StringW message);
    DECLARE_INSTANCE_METHOD(void, VideoPlayerPrepareComplete, UnityEngine::Video::VideoPlayer* source);
    DECLARE_INSTANCE_METHOD(void, VideoPlayerStarted, UnityEngine::Video::VideoPlayer* source);
    DECLARE_INSTANCE_METHOD(void, VideoPlayerFinished, UnityEngine::Video::VideoPlayer* source);

    DECLARE_INSTANCE_METHOD(float, get_playbackSpeed);
    DECLARE_INSTANCE_METHOD(void, set_playbackSpeed, float value);
    DECLARE_INSTANCE_METHOD(StringW, get_url);
    DECLARE_INSTANCE_METHOD(void, set_url, StringW value);
    DECLARE_INSTANCE_METHOD(void, set_volume, float value);
    DECLARE_INSTANCE_METHOD(bool, get_isPlaying);
    DECLARE_INSTANCE_METHOD(bool, get_isPrepared);
    DECLARE_INSTANCE_METHOD(void, set_time, double value);
    DECLARE_INSTANCE_METHOD(void, set_sendFrameReadyEvents, bool value);

    public:

    __declspec(property(get=get_playbackSpeed, put=set_playbackSpeed)) float playbackSpeed;
    __declspec(property(get=get_url, put=set_url)) StringW url;
    __declspec(property(put=set_volume)) float volume;
    __declspec(property(get=get_isPlaying)) bool isPlaying;
    __declspec(property(get=get_isPrepared)) bool isPrepared;
    __declspec(property(put=set_time)) double time;
    __declspec(property(put=set_sendFrameReadyEvents)) bool sendFrameReadyEvents;
)