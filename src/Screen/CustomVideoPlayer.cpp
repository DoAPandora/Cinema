#include "main.hpp"

#include "Screen/CustomVideoPlayer.hpp"

#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/TextureWrapMode.hpp"
#include "UnityEngine/Video/VideoAudioOutputMode.hpp"
#include "UnityEngine/Video/VideoRenderMode.hpp"
#include "UnityEngine/Video/VideoSource.hpp"

#include "assets.hpp"

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
        screenColorOn.a = 0;

        DEBUG("Creating CustomVideoPlayer");

        CreateScreen();
        screenRenderer = screenController->GetRenderer();
        screenRenderer->material = Material::New_ctor(GetShader());
        // screenRenderer->material->color = screenColorOff;
        screenRenderer->material->enableInstancing = true;
        player = gameObject->AddComponent<VideoPlayer*>();

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

        videoPlayerAudioSource = get_gameObject()->AddComponent<AudioSource*>();

        player->audioOutputMode = VideoAudioOutputMode::AudioSource;

        player->SetTargetAudioSource(0, videoPlayerAudioSource);

        Mute();
        //        screenController->SetScreensActive(false);

        screenController->EnableColorBlending(true);
        SetDefaultMenuPlacement();
    }

    void CustomVideoPlayer::SetDefaultMenuPlacement(std::optional<float> width)
    {
        Placement placement = Placement::MenuPlacement;
        placement.width = width.value_or(placement.height * (21.0f / 9.0f));
        SetPlacement(placement);
    }

    void CustomVideoPlayer::SetPlacement(Placement& placement)
    {
        screenController->SetPlacement(placement);
    }

    void CustomVideoPlayer::OnDestroy()
    {}

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

    void CustomVideoPlayer::FadeControllerUpdate(float value) {}

    void CustomVideoPlayer::OnMenuSceneLoaded() {}

    void CustomVideoPlayer::FirstFrameReady(UnityEngine::Video::VideoPlayer*, int64_t frame) {}

    void CustomVideoPlayer::SetBrightness(float brightness) {}

    void CustomVideoPlayer::SetBloomIntensity(std::optional<float> bloomIntensity) {}

    void CustomVideoPlayer::LoopVideo(bool loop) {}

    void CustomVideoPlayer::Show()
    {
        FadeIn(0);
    }

    void CustomVideoPlayer::FadeIn(float duration)
    {
        gameObject->active = true;
        DEBUG("FadeIn is not implemented!");
    }

    void CustomVideoPlayer::Hide()
    {
        FadeOut(0);
    }

    void CustomVideoPlayer::FadeOut(float duration)
    {
        // waitingForFadeOut = true;
        gameObject->active = false;
        Stop();
        DEBUG("FadeOut is not implemented!");
    }

    void CustomVideoPlayer::ShowScreenBody() {}

    void CustomVideoPlayer::HideScreenBody() {}

    void CustomVideoPlayer::CreateScreen()
    {
        screenController = ScreenController::New_ctor();
        screenController->CreateScreen(transform);
        screenController->SetScreensActive(true);
        SetDefaultMenuPlacement();
    }

    void CustomVideoPlayer::Play()
    {
        player->Play();
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
    {}

    void CustomVideoPlayer::VideoPlayerFinished(UnityEngine::Video::VideoPlayer* source)
    {}

    void CustomVideoPlayer::VideoPlayerErrorReceived(UnityEngine::Video::VideoPlayer* source, StringW message)
    {}

    float CustomVideoPlayer::GetVideoAspectRatio() {}

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
    }

    void CustomVideoPlayer::set_sendFrameReadyEvents(bool value)
    {
        player->sendFrameReadyEvents = value;
    }
} // namespace Cinema