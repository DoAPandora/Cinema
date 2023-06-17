#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-experimental-coroutine"

#include "main.hpp"
#include "Screen/VideoPlayer.hpp"

#include "questui/shared/ArrayUtil.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/WaitForSeconds.hpp"

#include "GlobalNamespace/SharedCoroutineStarter.hpp"

#include "custom-types/shared/coroutine.hpp"

using namespace UnityEngine;

namespace Cinema {

    custom_types::Helpers::Coroutine StartVideoCoroutine(Cinema::VideoPlayer* videoPlayer, float offset) {
        getLogger().info("Starting video with offset %f", offset);
        if(offset < 0)
        {
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForSeconds::New_ctor(offset * -1 + 0.6f));
        }
        else
            videoPlayer->set_time(offset + 0.6f);
        videoPlayer->Play();
        co_return;
    }

    void VideoPlayer::Start(float offset)
    {
        Prepare();
        GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(StartVideoCoroutine(this, offset)));
    }

    void VideoPlayer::Awake()
    {
        getLogger().info("VideoPlayer::Awake");
    }

    VideoPlayer* VideoPlayer::CreateVideoPlayer()
    {
        getLogger().info("Creating VideoPlayer");
        GameObject* screenGo = GameObject::CreatePrimitive(PrimitiveType::Plane);
        GameObject::DontDestroyOnLoad(screenGo);

        auto material = Resources::FindObjectsOfTypeAll<Material*>().LastOrDefault([](Material* x) {
            return x->get_name() == "PyroVideo (Instance)";
        });
        
        if(material)
            screenGo->GetComponent<Renderer*>()->set_material(material);
        else
            screenGo->GetComponent<Renderer*>()->set_material(Material::New_ctor(Shader::Find(il2cpp_utils::newcsstr("Unlit/Texture"))));
        screenGo->get_transform()->set_position(Vector3{0.0f, 12.4f, 67.8f});
        screenGo->get_transform()->set_rotation(Quaternion::Euler(90.0f, 270.0f, 90.0f));
        screenGo->get_transform()->set_localScale(Vector3(5.11, 1, 3));

        auto videoPlayer = screenGo->AddComponent<Cinema::VideoPlayer*>();
        videoPlayer->screenGo = screenGo;
        videoPlayer->set_isLooping(true);
        videoPlayer->set_playOnAwake(false);
        videoPlayer->set_renderMode(Video::VideoRenderMode::MaterialOverride);
        videoPlayer->set_audioOutputMode(Video::VideoAudioOutputMode::None);
        videoPlayer->set_aspectRatio(Video::VideoAspectRatio::FitInside);

        auto cinemaScreen = screenGo->GetComponent<Renderer*>();
        if(cinemaScreen)
            videoPlayer->set_renderer(cinemaScreen);

        return videoPlayer;
    }
}
#pragma clang diagnostic pop