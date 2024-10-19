#include "Screen/ScreenController.hpp"
#include "main.hpp"

#include "Screen/CustomVideoPlayer.hpp"

#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/TextureWrapMode.hpp"
#include "UnityEngine/Video/VideoAudioOutputMode.hpp"
#include "UnityEngine/Video/VideoRenderMode.hpp"
#include "UnityEngine/Video/VideoSource.hpp"

#include "custom-types/shared/delegate.hpp"

#include "assets.hpp"
#include <functional>

using namespace UnityEngine;
using namespace UnityEngine::Video;

DEFINE_TYPE(Cinema, CustomVideoPlayer);


template <typename T>
requires(std::is_convertible_v<T, System::MulticastDelegate*>)
    T
    operator+=(T l, T r)
{
    return reinterpret_cast<T>(System::Delegate::Combine(l, r));
}

namespace Cinema
{

    void CustomVideoPlayer::Awake()
    {
        DEBUG("Creating CustomVideoPlayer");

        CreateScreen();
        screenRenderer = screenController->GetRenderer();
        screenRenderer->material = Material::New_ctor(GetShader());
        // screenRenderer->material->color = screenColorOff;
        screenRenderer->material->enableInstancing = true;
        player = gameObject->AddComponent<VideoPlayer*>();
        player->set_sendFrameReadyEvents(true);

        player->source = Video::VideoSource::Url;

        player->renderMode = Video::VideoRenderMode::RenderTexture;
        renderTexture = screenController->CreateRenderTexture();
        renderTexture->wrapMode = TextureWrapMode::Mirror;

        player->targetTexture = renderTexture;
        player->playOnAwake = false;
        player->waitForFirstFrame = true;

        videoPlayerErrorReceived = custom_types::MakeDelegate<Video::VideoPlayer::ErrorEventHandler*>(
            std::function<void(Video::VideoPlayer*, StringW)>(
                std::bind_front(&CustomVideoPlayer::VideoPlayerErrorReceived, this)));
        player->errorReceived = (Video::VideoPlayer::ErrorEventHandler*)System::Delegate::Combine(player->errorReceived, videoPlayerErrorReceived);

        videoPlayerPrepareComplete = custom_types::MakeDelegate<VideoPlayer::EventHandler*>(
            std::function<void(VideoPlayer*)>(
                std::bind_front(&CustomVideoPlayer::VideoPlayerPrepareComplete, this)));
        player->prepareCompleted = (VideoPlayer::EventHandler*)System::Delegate::Combine(player->prepareCompleted, videoPlayerPrepareComplete);

        videoPlayerStarted = custom_types::MakeDelegate<VideoPlayer::EventHandler*>(
            std::function<void(VideoPlayer*)>(
                std::bind_front(&CustomVideoPlayer::VideoPlayerStarted, this)));
        player->started = (VideoPlayer::EventHandler*)System::Delegate::Combine(player->started, videoPlayerStarted);

        videoPlayerFinished = custom_types::MakeDelegate<VideoPlayer::EventHandler*>(
            std::function<void(VideoPlayer*)>(
                std::bind_front(&CustomVideoPlayer::VideoPlayerFinished, this)));
        player->loopPointReached = (VideoPlayer::EventHandler*)System::Delegate::Combine(player->loopPointReached, videoPlayerFinished);

        videoPlayerFirstFrameReady = custom_types::MakeDelegate<VideoPlayer::FrameReadyEventHandler*>(
            std::function<void(VideoPlayer*, uint64_t)>(
                std::bind_front(&CustomVideoPlayer::FirstFrameReady, this)));

        videoPlayerAudioSource = get_gameObject()->AddComponent<AudioSource*>();

        player->audioOutputMode = VideoAudioOutputMode::AudioSource;

        player->SetTargetAudioSource(0, videoPlayerAudioSource);

        Mute();
        //        screenController->SetScreensActive(false);

        screenController->EnableColorBlending(true);
        
        fadeController.easingUpdate += {&CustomVideoPlayer::FadeControllerUpdate, this};

        SetDefaultMenuPlacement();
    }

    void CustomVideoPlayer::SetDefaultMenuPlacement(std::optional<float> width)
    {
        Placement placement = Placement::MenuPlacement;
        placement.width = width.value_or(placement.height * (21.0f / 9.0f));
        SetPlacement(placement);
    }

    void CustomVideoPlayer::SetPlacement(const Placement& placement)
    {
        screenController->SetPlacement(placement);
    }

    void CustomVideoPlayer::OnDestroy()
    {
        fadeController.easingUpdate -= {&CustomVideoPlayer::FadeControllerUpdate, this};
        renderTexture->Release();
    }

    Shader* CustomVideoPlayer::GetShader()
    {
        using AssetBundle_LoadFromMemory = function_ptr_t<UnityEngine::AssetBundle*, ArrayW<uint8_t>, int>;
        static AssetBundle_LoadFromMemory assetBundle_LoadFromMemory = reinterpret_cast<AssetBundle_LoadFromMemory>(il2cpp_functions::resolve_icall("UnityEngine.AssetBundle::LoadFromMemory_Internal"));

        UnityW<AssetBundle> bundle = assetBundle_LoadFromMemory(IncludedAssets::videoshader, 0);
        if(!bundle)
        {
            ERROR("Loading asset bundle failed!");
            return Shader::Find("Hidden/BlitAdd");
        }

        Shader* shader = bundle->LoadAsset<Shader*>("assets/videoshader.shader");
        bundle->Unload(false);

        return shader;
    }

    void CustomVideoPlayer::FadeControllerUpdate(float value)
    {
        ScreenColor = screenColorOn * value;
        if(!muted)
        {
            Volume = MAX_VOLUME * volumeScale * value;
        }

        if(value >= 1 && bodyVisible)
        {
            screenController->SetScreenBodiesActive(true);
        }
        else
        {
            screenController->SetScreenBodiesActive(false);
        }

        if(value == 0 && player->url == currentlyPlayingVideo && waitingForFadeOut)
        {
            Stop();
        }
    }

    void CustomVideoPlayer::OnMenuSceneLoaded() {}

    void CustomVideoPlayer::FirstFrameReady(UnityEngine::Video::VideoPlayer*, int64_t frame)
    {
        FadeIn();
        screenController->SetAspectRatio(GetVideoAspectRatio());
        player->remove_frameReady(videoPlayerFirstFrameReady);
    }

    void CustomVideoPlayer::SetBrightness(float brightness)
    {
        fadeController.value = brightness;
    }

    void CustomVideoPlayer::SetBloomIntensity(std::optional<float> bloomIntensity) {}

    void CustomVideoPlayer::LoopVideo(bool loop) {}

    void CustomVideoPlayer::Show()
    {
        FadeIn(0);
    }

    void CustomVideoPlayer::FadeIn(float duration)
    {
        screenController->SetScreensActive(true);
        waitingForFadeOut = false;
        fadeController.EaseIn(duration);
    }

    void CustomVideoPlayer::Hide()
    {
        FadeOut(0);
    }

    void CustomVideoPlayer::FadeOut(float duration)
    {
        waitingForFadeOut = true;
        fadeController.EaseOut(duration);
    }

    void CustomVideoPlayer::ShowScreenBody()
    {
        bodyVisible = true;
        if(!fadeController.IsFading() && fadeController.IsOne())
        {
            screenController->SetScreenBodiesActive(true);
        }
    }

    void CustomVideoPlayer::HideScreenBody()
    {
        bodyVisible = true;
        if(!fadeController.IsFading())
        {
            screenController->SetScreenBodiesActive(false);
        }
    }

    void CustomVideoPlayer::CreateScreen()
    {
        screenController = ScreenController::New_ctor();
        screenController->CreateScreen(transform);
        screenController->SetScreensActive(true);
        SetDefaultMenuPlacement();
    }

    void CustomVideoPlayer::Play()
    {
        player->remove_frameReady(videoPlayerFirstFrameReady);
        player->add_frameReady(videoPlayerFirstFrameReady);
        player->Play();
        Shader::SetGlobalInt(CinemaStatusProperty, 1);
    }

    void CustomVideoPlayer::Pause()
    {
        player->Pause();
    }

    void CustomVideoPlayer::Stop()
    {
        player->Stop();
        SetStaticTexture(nullptr);
    }

    void CustomVideoPlayer::Prepare()
    {
        player->Prepare();
    }

    void CustomVideoPlayer::Update()
    {
        if(player->isPlaying /*|| (player->IsPrepared && player->IsPaused)*/)
        {
            SetTexture(player->texture);
        }
    }

    void CustomVideoPlayer::UpdateScreenContent() {}

    void CustomVideoPlayer::SetTexture(UnityEngine::Texture* texture)
    {
        Shader::SetGlobalTexture(CinemaVideoTexture, texture);
    }

    void CustomVideoPlayer::SetCoverTexture(UnityEngine::Texture* texture) {}

    void CustomVideoPlayer::SetStaticTexture(UnityEngine::Texture* texture) {}

    void CustomVideoPlayer::ClearTexture() {}

    void CustomVideoPlayer::VideoPlayerPrepareComplete(UnityEngine::Video::VideoPlayer* source)
    {}

    void CustomVideoPlayer::VideoPlayerStarted(UnityEngine::Video::VideoPlayer* source)
    {
        currentlyPlayingVideo = static_cast<std::string>(source->url);
        waitingForFadeOut = false;
        videoEnded = false;
    }

    void CustomVideoPlayer::VideoPlayerFinished(UnityEngine::Video::VideoPlayer* source)
    {}

    void CustomVideoPlayer::VideoPlayerErrorReceived(UnityEngine::Video::VideoPlayer* source, StringW message)
    {}

    float CustomVideoPlayer::GetVideoAspectRatio()
    {
        auto texture = player->get_texture();
        if(texture && texture->get_width() != 0 && texture->get_height() != 0)
        {
            float aspectRatio = (float)texture->get_width() / (float)texture->get_height();
            return aspectRatio;
        }

        DEBUG("Using default aspect ratio (texture missing)");
        return 16.0f / 9.0f;
    }

    void CustomVideoPlayer::Mute() {}

    void CustomVideoPlayer::Unmute() {}

    void CustomVideoPlayer::SetSoftParent(UnityEngine::Transform* parent) {}

    Color CustomVideoPlayer::get_ScreenColor()
    {
        return screenRenderer->get_material()->get_color();
        return Color::get_clear();
    }

    void CustomVideoPlayer::set_ScreenColor(UnityEngine::Color value)
    {
        screenRenderer->get_material()->set_color(value);
    }

    float CustomVideoPlayer::get_PlaybackSpeed()
    {
        return player->playbackSpeed;
    }

    void CustomVideoPlayer::set_PlaybackSpeed(float value)
    {
        player->playbackSpeed = value;
    }

    float CustomVideoPlayer::get_VideoDuration()
    {
    }

    void CustomVideoPlayer::set_Volume(float value)
    {
        videoPlayerAudioSource->volume = value;
    }

    void CustomVideoPlayer::set_PanStereo(float value)
    {
        videoPlayerAudioSource->panStereo = value;
    }

    StringW CustomVideoPlayer::get_Url()
    {
        return player->url;
    }

    void CustomVideoPlayer::set_Url(StringW value)
    {
        player->url = value;
    }

    bool CustomVideoPlayer::get_IsPlaying()
    {
        return player->get_isPlaying();
    }

    bool CustomVideoPlayer::get_IsFading() {}

    bool CustomVideoPlayer::get_IsPrepared()
    {
        return player->isPrepared;
    }

    void CustomVideoPlayer::set_time(double value)
    {
        player->time = value;
        DEBUG("set time to {}", value);
    }

    void CustomVideoPlayer::set_sendFrameReadyEvents(bool value)
    {
        player->sendFrameReadyEvents = value;
    }
} // namespace Cinema