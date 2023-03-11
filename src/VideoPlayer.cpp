#include "main.hpp"
#include "VideoPlayer.hpp"

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

    custom_types::Helpers::Coroutine coroutine(Cinema::VideoPlayer* videoPlayer, AudioSource* audioSource, float offset) {
        while(!audioSource->get_isPlaying()) co_yield nullptr;
        getLogger().info("Starting video with offset %f", offset);
        if(0 > offset)
        {
            co_yield reinterpret_cast<System::Collections::IEnumerator*>(UnityEngine::WaitForSeconds::New_ctor(offset * -1 + 0.5f));
        }
        else
            videoPlayer->set_time(offset);
        videoPlayer->Play();
        co_return;
    }

    void VideoPlayer::Start(float offset)
    {
        this->Prepare();
        GlobalNamespace::SharedCoroutineStarter::get_instance()->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(coroutine(this, this->audioSource, offset)));
    }

    VideoPlayer* VideoPlayer::CreateVideoPlayer(UnityEngine::AudioSource* _audioSource)
    {
        GameObject* Mesh = GameObject::CreatePrimitive(PrimitiveType::Plane);
        auto material = QuestUI::ArrayUtil::Last(Resources::FindObjectsOfTypeAll<Material*>(), [](Material* x) {
            return x->get_name() == "PyroVideo (Instance)";
        });
        if(material)
            Mesh->GetComponent<Renderer*>()->set_material(material);
        else
            Mesh->GetComponent<Renderer*>()->set_material(Material::New_ctor(Shader::Find(il2cpp_utils::newcsstr("Unlit/Texture"))));
        Mesh->get_transform()->set_position(Vector3{0.0f, 12.4f, 67.8f});
        Mesh->get_transform()->set_rotation(Quaternion::Euler(90.0f, 270.0f, 90.0f));
        Mesh->get_transform()->set_localScale(Vector3(5.11, 1, 3));

        auto cinemaScreen = Mesh->GetComponent<Renderer*>();

        auto videoPlayer = Mesh->AddComponent<Cinema::VideoPlayer*>();
        videoPlayer->set_isLooping(true);
        videoPlayer->set_playOnAwake(false);
        videoPlayer->set_renderMode(Video::VideoRenderMode::MaterialOverride);
        videoPlayer->set_audioOutputMode(Video::VideoAudioOutputMode::None);
        videoPlayer->set_aspectRatio(Video::VideoAspectRatio::FitInside);
        if(cinemaScreen)
            videoPlayer->set_renderer(cinemaScreen);
        // videoPlayer->set_url(Cinema::VideoManager::GetCurrentVideoPath());
        videoPlayer->audioSource = _audioSource;

        // videoPlayer->Prepare();
        return videoPlayer;
    }
}