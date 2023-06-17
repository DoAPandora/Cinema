#pragma once
#include "UnityEngine/Video/VideoPlayer.hpp"
#include "UnityEngine/Video/VideoRenderMode.hpp"
#include "UnityEngine/Video/VideoAudioOutputMode.hpp"
#include "UnityEngine/Video/VideoAspectRatio.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/AudioSource.hpp"

namespace Cinema {
    class VideoPlayer : public UnityEngine::Video::VideoPlayer {

    public:        

        UnityEngine::GameObject* screenGo;

        static VideoPlayer* CreateVideoPlayer();

        void Start(float offset);

        void Awake();

        void set_time(double time) {
            static auto setTime = reinterpret_cast<function_ptr_t<void, UnityEngine::Video::VideoPlayer*, double>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_time"));
            setTime(this, time);
        }

        void set_aspectRatio(UnityEngine::Video::VideoAspectRatio ratio) {
            static auto aspectRatio = reinterpret_cast<function_ptr_t<void, UnityEngine::Video::VideoPlayer*, UnityEngine::Video::VideoAspectRatio>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_aspectRatio"));
            aspectRatio(this, ratio);
        }

        bool get_isPrepared() {
            static auto isPrepared = reinterpret_cast<function_ptr_t<bool, UnityEngine::Video::VideoPlayer*>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::get_isPrepared"));
            return isPrepared(this);
        }

        void Pause() {
            static auto pauseVideo = reinterpret_cast<function_ptr_t<void, UnityEngine::Video::VideoPlayer*>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::Pause"));
            pauseVideo(this);
        }

        void Prepare() {
            static auto prepareVideo = reinterpret_cast<function_ptr_t<void, UnityEngine::Video::VideoPlayer*>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::Prepare"));
            prepareVideo(this);
        }

        void set_audioOutputMode(UnityEngine::Video::VideoAudioOutputMode mode) {
            static auto audioOutputMode = reinterpret_cast<function_ptr_t<void, UnityEngine::Video::VideoPlayer*, UnityEngine::Video::VideoAudioOutputMode>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_audioOutputMode"));
            audioOutputMode(this, mode);
        }

        void set_playOnAwake(bool value) {
            static auto playOnAwake = reinterpret_cast<function_ptr_t<void, UnityEngine::Video::VideoPlayer*, bool>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_playOnAwake"));
            playOnAwake(this, value);
        }

        void set_isLooping(bool value) {
            static auto isLooping = reinterpret_cast<function_ptr_t<void, UnityEngine::Video::VideoPlayer*, bool>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_isLooping"));
            isLooping(this, value);

        }

        void set_renderMode(UnityEngine::Video::VideoRenderMode renderMode) {
            static auto setRenderMode = reinterpret_cast<function_ptr_t<void, UnityEngine::Video::VideoPlayer*, UnityEngine::Video::VideoRenderMode>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_renderMode"));
            setRenderMode(this, renderMode);
        }

        void set_renderer(UnityEngine::Renderer* renderer) {
            static auto set_targetRenderer = reinterpret_cast<function_ptr_t<void, UnityEngine::Video::VideoPlayer*, UnityEngine::Renderer*>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_targetMaterialRenderer"));
            set_targetRenderer(this, renderer);
        }

        void set_url(StringW url) {
            static auto setUrl = reinterpret_cast<function_ptr_t<void, UnityEngine::Video::VideoPlayer*, StringW>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::set_url"));
            setUrl(this, url);
        }
    };
}
DEFINE_IL2CPP_ARG_TYPE(Cinema::VideoPlayer*, "UnityEngine.Video", "VideoPlayer");