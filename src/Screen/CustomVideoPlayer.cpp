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

#define RESOLVE_ICALL(name, ret, ...) reinterpret_cast<function_ptr_t<ret, UnityEngine::Video::VideoPlayer*, ##__VA_ARGS__>>(il2cpp_functions::resolve_icall("UnityEngine.Video.VideoPlayer::" #name));

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

#ifdef USE_CURVED_SCREEN
        CreateScreen();
        screenRenderer = screenController->GetRenderer();
        screenRenderer->material = Material::New_ctor(GetShader());
        screenRenderer->material->color = screenColorOff;
        screenRenderer->material->enableInstancing = true;
        player = gameObject->AddComponent<VideoPlayer*>();
#else
        player = CreateVideoPlayer(transform);
#endif

        auto set_source = RESOLVE_ICALL(set_source, void, Video::VideoSource);
        set_source(player, Video::VideoSource::Url);

#ifdef USE_CURVED_SCREEN
        auto set_renderMode = RESOLVE_ICALL(set_renderMode, void, Video::VideoRenderMode);
        set_renderMode(player, Video::VideoRenderMode::RenderTexture);
        renderTexture = screenController->CreateRenderTexture();
        renderTexture->wrapMode = TextureWrapMode::Mirror;

        auto set_targetTexture = RESOLVE_ICALL(set_targetTexture, void, RenderTexture*);
        set_targetTexture(player, renderTexture);
#endif
        auto set_playOnAwake = RESOLVE_ICALL(set_playOnAwake, void, bool);
        set_playOnAwake(player, false);

        auto set_waitForFirstFrame = RESOLVE_ICALL(set_waitForFirstFrame, void, bool);
        set_waitForFirstFrame(player, true);

        videoPlayerErrorReceived = custom_types::MakeDelegate<Video::VideoPlayer::ErrorEventHandler*>(
            std::function<void(Video::VideoPlayer*, StringW)>(
                std::bind(&CustomVideoPlayer::VideoPlayerErrorReceived, this, std::placeholders::_1, std::placeholders::_2)));
        player->errorReceived = (Video::VideoPlayer::ErrorEventHandler*)System::Delegate::Combine(player->errorReceived, videoPlayerErrorReceived);

        videoPlayerPrepareComplete = custom_types::MakeDelegate<VideoPlayer::EventHandler*>(
            std::function<void(VideoPlayer*)>(
                std::bind(&CustomVideoPlayer::VideoPlayerPrepareComplete, this, std::placeholders::_1)));
        player->prepareCompleted = (VideoPlayer::EventHandler*)System::Delegate::Combine(player->prepareCompleted, videoPlayerPrepareComplete);

        videoPlayerStarted = custom_types::MakeDelegate<VideoPlayer::EventHandler*>(
            std::function<void(VideoPlayer*)>(
                std::bind(&CustomVideoPlayer::VideoPlayerStarted, this, std::placeholders::_1)));
        player->started = (VideoPlayer::EventHandler*)System::Delegate::Combine(player->started, videoPlayerStarted);

        videoPlayerFinished = custom_types::MakeDelegate<VideoPlayer::EventHandler*>(
            std::function<void(VideoPlayer*)>(
                std::bind(&CustomVideoPlayer::VideoPlayerFinished, this, std::placeholders::_1)));
        player->loopPointReached = (VideoPlayer::EventHandler*)System::Delegate::Combine(player->loopPointReached, videoPlayerFinished);

        videoPlayerAudioSource = get_gameObject()->AddComponent<AudioSource*>();

        auto set_audioOutputMode = RESOLVE_ICALL(set_audioOutputMode, void, Video::VideoAudioOutputMode);
        set_audioOutputMode(player, Video::VideoAudioOutputMode::AudioSource);

        auto SetTargetAudioSource = RESOLVE_ICALL(SetTargetAudioSource, void, uint16_t, AudioSource*);
        SetTargetAudioSource(player, 0, videoPlayerAudioSource);

        Mute();
        //        screenController->SetScreensActive(false);

#ifdef USE_CURVED_SCREEN
        screenController->EnableColorBlending(true);
#endif
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
// will add this properly soon ?
#ifdef USE_CURVED_SCREEN
        screenController->SetPlacement(placement);
#endif
    }

    // Flat plane video player
    Video::VideoPlayer* CustomVideoPlayer::CreateVideoPlayer(UnityEngine::Transform* parent)
    {
        INFO("Creating Flat Plane VideoPlayer");
        GameObject* screenGo = GameObject::CreatePrimitive(PrimitiveType::Plane);
        screenGo->get_transform()->set_parent(parent);
        GameObject::DontDestroyOnLoad(screenGo);

        auto material = Resources::FindObjectsOfTypeAll<Material*>().back_or_default([](Material* x)
                                                                                     { return x->get_name() == "PyroVideo (Instance)"; });

        if(material)
            screenGo->GetComponent<Renderer*>()->set_material(material);
        else
            screenGo->GetComponent<Renderer*>()->set_material(Material::New_ctor(Shader::Find("Unlit/Texture")));
        screenGo->get_transform()->set_position(Vector3{0.0f, 12.4f, 67.8f});
        screenGo->get_transform()->set_rotation(Quaternion::Euler(90.0f, 270.0f, 90.0f));
        screenGo->get_transform()->set_localScale(Vector3(5.11, 1, 3));

        auto videoPlayer = screenGo->AddComponent<Video::VideoPlayer*>();

        auto cinemaScreen = screenGo->GetComponent<Renderer*>();
        if(cinemaScreen)
        {
            static auto set_targetMaterialRenderer = RESOLVE_ICALL(set_targetMaterialRenderer, void, Renderer*);
            set_targetMaterialRenderer(videoPlayer, cinemaScreen);
        }

        return videoPlayer;
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

    void CustomVideoPlayer::FadeIn(float duration) {}

    void CustomVideoPlayer::Hide()
    {
        FadeOut(0);
    }

    void CustomVideoPlayer::FadeOut(float duration)
    {
        waitingForFadeOut = true;
    }

    void CustomVideoPlayer::ShowScreenBody() {}

    void CustomVideoPlayer::HideScreenBody() {}

    void CustomVideoPlayer::CreateScreen()
    {
#ifdef USE_CURVED_SCREEN
        screenController = ScreenController::New_ctor();
        screenController->CreateScreen(transform);
        screenController->SetScreensActive(true);
        SetDefaultMenuPlacement();
#else

#endif
    }

    void CustomVideoPlayer::Play()
    {
        player->Play();
    }

    void CustomVideoPlayer::Pause()
    {
        static auto func = RESOLVE_ICALL(Pause, void);
        func(player);
    }

    void CustomVideoPlayer::Stop() {}

    void CustomVideoPlayer::Prepare()
    {
        static auto func = RESOLVE_ICALL(Prepare, void);
        func(player);
    }

    void CustomVideoPlayer::Update() {}

    void CustomVideoPlayer::UpdateScreenContent() {}

    void CustomVideoPlayer::SetTexture(UnityEngine::Texture* texture) {}

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
#ifdef USE_CURVED_SCREEN
        return screenRenderer->get_material()->get_color();
#else
        return Color::get_clear();
#endif
    }

    void CustomVideoPlayer::set_ScreenColor(UnityEngine::Color value)
    {
#ifdef USE_CURVED_SCREEN
        screenRenderer->get_material()->set_color(value);
#endif
    }

    float CustomVideoPlayer::get_PlaybackSpeed()
    {
        static auto func = RESOLVE_ICALL(get_playbackSpeed, float);
        return func(player);
    }

    void CustomVideoPlayer::set_PlaybackSpeed(float value)
    {
        player->set_playbackSpeed(value);
    }

    float CustomVideoPlayer::get_VideoDuration()
    {
    }

    void CustomVideoPlayer::set_Volume(float value)
    {
        static auto func = reinterpret_cast<function_ptr_t<void, AudioSource*, float>>(il2cpp_functions::resolve_icall("UnityEngine.AudioSource::set_volume"));
        func(videoPlayerAudioSource, value);
    }

    void CustomVideoPlayer::set_PanStereo(float value)
    {
        static auto func = reinterpret_cast<function_ptr_t<void, AudioSource*, float>>(il2cpp_functions::resolve_icall("UnityEngine.AudioSource::set_panStereo"));
        func(videoPlayerAudioSource, value);
    }

    StringW CustomVideoPlayer::get_Url()
    {
        static auto func = RESOLVE_ICALL(get_url, StringW);
        return func(player);
    }

    void CustomVideoPlayer::set_Url(StringW value)
    {
        static auto func = RESOLVE_ICALL(set_url, void, StringW);
        func(player, value);
    }

    bool CustomVideoPlayer::get_IsPlaying()
    {
        return player->get_isPlaying();
    }

    bool CustomVideoPlayer::get_IsFading() {}

    bool CustomVideoPlayer::get_IsPrepared()
    {
        static auto func = RESOLVE_ICALL(get_isPrepared, bool);
        return func(player);
    }

    void CustomVideoPlayer::set_time(double value)
    {
        static auto func = RESOLVE_ICALL(set_time, void, double);
        func(player, value);
    }

    void CustomVideoPlayer::set_sendFrameReadyEvents(bool value)
    {
        static auto func = RESOLVE_ICALL(set_sendFrameReadyEvents, void, bool);
        func(player, value);
    }
} // namespace Cinema