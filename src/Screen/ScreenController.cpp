#include "Screen/ScreenController.hpp"
#include "Screen/CurvedSurface.hpp"
#include "Util/SoftParent.hpp"
#include "main.hpp"

#include "UnityEngine/LayerMask.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/MeshRenderer.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/RenderTextureFormat.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine_Rendering_BlendMode.hpp"

using namespace UnityEngine;

DEFINE_TYPE(Cinema, ScreenController);

namespace Cinema
{

    void ScreenController::ctor()
    {
        INVOKE_CTOR();
        INVOKE_BASE_CTOR(classof(System::Object*));
        materialPropertyBlock = MaterialPropertyBlock::New_ctor();
    }

    void ScreenController::CreateScreen(Transform* parent)
    {
        GameObject* newScreen = GameObject::New_ctor("CinemaScreen");
        newScreen->transform->parent = parent->transform;
        newScreen->AddComponent<CurvedSurface*>();
        newScreen->AddComponent<MeshRenderer*>();
        newScreen->AddComponent<MeshFilter*>();
        newScreen->layer = LayerMask::NameToLayer("Environment");
        CreateScreenBody(newScreen->transform);
        newScreen->AddComponent<SoftParent*>();
        screens->Add(newScreen);
    }

    void ScreenController::CreateScreenBody(Component* parent)
    {
        GameObject* body = GameObject::New_ctor("Body");
        body->AddComponent<CurvedSurface*>();
        body->AddComponent<MeshRenderer*>();
        body->AddComponent<MeshFilter*>();
        body->transform->parent = parent->transform;
        body->transform->localPosition = Vector3(0, 0, 0.4f);
        body->transform->localScale = Vector3(1.01f, 1.01f, 1.01f);
        body->layer = LayerMask::NameToLayer("Environment");
    }

    void ScreenController::OnGameSceneLoadedFresh()
    {
        for(auto screen : screens)
        {
            Transform* body = screen->transform->Find("Body");
            Renderer* bodyRenderer = body->GetComponent<Renderer*>();
            if(bodyRenderer == nullptr)
            {
                ERROR("Could not find body renderer for screen");
                return;
            }

            if(!bodyRenderer->material || !bodyRenderer->material->shader || bodyRenderer->material->shader->name != BODY_SHADER_NAME)
            {
                AssignBodyMaterial(bodyRenderer);
            }
        }
    }

    void ScreenController::AssignBodyMaterial(Renderer* bodyRenderer)
    {
        UnityW<Shader> bodyShader = Resources::FindObjectsOfTypeAll<Shader*>().back_or_default([this](Shader* x)
                                                                                               { return x->name == BODY_SHADER_NAME; });
        if(bodyShader)
        {
            bodyRenderer->material = Material::New_ctor(bodyShader);
        }

        bodyRenderer->material->color = Color(0, 0, 0, 0);
    }

    void ScreenController::SetScreensActive(bool active)
    {
        for(GameObject* screen : screens)
        {
            screen->SetActive(active);
        }
    }

    void ScreenController::SetScreenBodiesActive(bool active)
    {
        for(GameObject* screen : screens)
        {
            screen->transform->GetChild(0)->gameObject->SetActive(active);
        }
    }

    Renderer* ScreenController::GetRenderer()
    {
        return screens[0]->GetComponent<Renderer*>();
    }

    RenderTexture* ScreenController::CreateRenderTexture()
    {
        RenderTexture* renderTexture = RenderTexture::New_ctor(1920, 1080, 24, RenderTextureFormat::ARGB32);
        renderTexture->Create();
        return renderTexture;
    }

    void ScreenController::SetPlacement(Cinema::Placement& placement)
    {
        SetPlacement(placement.position, placement.rotation, placement.width, placement.height, placement.curvature, placement.subsurfaces, placement.curveYAxis);
    }

    void ScreenController::SetPlacement(UnityEngine::Vector3 position, UnityEngine::Vector3 rotation, float width, float height, std::optional<float> curvatureDegrees, std::optional<int> subsurfaces, std::optional<bool> curveYAxis)
    {
        GameObject* screen = screens[0];
        screen->transform->position = position;
        screen->transform->eulerAngles = rotation;
        screen->transform->localScale = Vector3::get_one();

        float distance = Vector3::Distance(screen->transform->position, Vector3::get_zero());
        float bodyDistance = std::max(0.05f, distance / 250.0f);
        screen->transform->Find("Body")->localPosition = Vector3(0, 0, bodyDistance);

        InitializeSurfaces(width, height, position.z, curvatureDegrees, subsurfaces, curveYAxis);
        RegenerateScreenSurfaces();
    }

    void ScreenController::InitializeSurfaces(float width, float height, float distance, std::optional<float> curvatureDegrees, std::optional<int> subsurfaces, std::optional<bool> curveYAxis)
    {
        for(GameObject* screen : screens)
        {
            CurvedSurface* screenSurface = screen->GetComponent<CurvedSurface*>();
            CurvedSurface* screenBodySurface = screen->transform->GetChild(0)->GetComponent<CurvedSurface*>();
            // get customm bloom

            screenSurface->Initialise(width, height, distance, curvatureDegrees, subsurfaces, curveYAxis);
            screenBodySurface->Initialise(width, height, distance, curvatureDegrees, subsurfaces, curveYAxis);
        }
    }

    void ScreenController::RegenerateScreenSurfaces()
    {
        for(GameObject* screen : screens)
        {
            screen->GetComponent<CurvedSurface*>()->Generate();
            screen->transform->GetChild(0)->GetComponent<CurvedSurface*>()->Generate();
            // Update custom bloom
        }
    }

    void ScreenController::SetBloomIntensity(std::optional<float> bloomIntensity)
    {
        // iterate screens and set bloom
    }

    void ScreenController::SetAspectRatio(float ratio)
    {
        for(GameObject* screen : screens)
        {
            CurvedSurface* screenSurface = screen->GetComponent<CurvedSurface*>();
            CurvedSurface* screenBodySurface = screen->transform->GetChild(0)->GetComponent<CurvedSurface*>();
            // get customm bloom

            screenSurface->Width = screenSurface->Height * ratio;
            screenBodySurface->Width = screenSurface->Height * ratio;
        }

        RegenerateScreenSurfaces();
    }

    void ScreenController::SetSoftParent(Transform* parent)
    {
        SoftParent* softParent = screens[0]->GetComponent<SoftParent*>();
        softParent->enabled = parent != nullptr;
        softParent->AssignParent(parent);
    }

    void ScreenController::SetShaderParameters(VideoConfigPtr config)
    {
        for(GameObject* screen : screens)
        {
            Renderer* screenRenderer = screen->GetComponent<Renderer*>();

            std::optional<VideoConfig::ColorCorrection> colorCorrection = config->colorCorrection;
            std::optional<VideoConfig::Vigenette> vigenette = config->vigenette;

            screenRenderer->GetPropertyBlock(materialPropertyBlock);

            SetShaderFloat(Brightness, colorCorrection->brightness, 0, 2, 1);
            SetShaderFloat(Contrast, colorCorrection->contrast, 0, 5, 1);
            SetShaderFloat(Saturation, colorCorrection->saturation, 0, 5, 1);
            SetShaderFloat(Hue, colorCorrection->hue, -360, 360, 0);
            SetShaderFloat(Exposure, colorCorrection->exposure, 0, 5, 1);
            SetShaderFloat(Gamma, colorCorrection->gamma, 0, 5, 1);

            EnableColorBlending(config->colorBlending.value_or(false));
            SetVigenette(vigenette, materialPropertyBlock);

            screenRenderer->SetPropertyBlock(materialPropertyBlock);
        }
    }

    void ScreenController::SetVigenette(std::optional<VideoConfig::Vigenette> vigenette, UnityEngine::MaterialPropertyBlock* materialPropertyBlock)
    {
        for(GameObject* screen : screens)
        {
            Renderer* screenRenderer = screen->GetComponent<Renderer*>();
            bool setPropertyBlock = materialPropertyBlock == nullptr;
            if(setPropertyBlock)
            {
                screenRenderer->GetPropertyBlock(this->materialPropertyBlock);
                materialPropertyBlock = this->materialPropertyBlock;
            }

            auto elliptical = vigenette == std::nullopt;
            SetShaderFloat(VignetteRadius, vigenette->radius, 0, 1, 1);
            SetShaderFloat(VignetteSoftness, vigenette->softness, 0, 1, elliptical ? 0.02f : 0.005f);
        }
    }

    void ScreenController::SetShaderFloat(int nameID, std::optional<float> value, float min, float max, float defaultValue)
    {
        materialPropertyBlock->SetFloat(nameID, std::clamp(value.value_or(defaultValue), min, max));
    }

    void ScreenController::EnableColorBlending(bool enable)
    {
        DEBUG("Enabling color blending: {}", enable);
        Renderer* screenRenderer = screens[0]->GetComponent<Renderer*>();
        SetBlendMode(enable ? BlendMode::SoftAdditive : BlendMode::PerfectVisibility, screenRenderer->material);
    }

    void ScreenController::SetBlendMode(Cinema::ScreenController::BlendMode blendMode, UnityEngine::Material* material)
    {
        switch(blendMode)
        {
        case BlendMode::SoftAdditive:
            {
                material->SetInt(SrcColor, (int)Rendering::BlendMode::OneMinusDstColor);
                material->SetInt(DestColor, (int)Rendering::BlendMode::One);
                material->SetInt(SrcAlpha, (int)Rendering::BlendMode::OneMinusDstColor);
                material->SetInt(DestAlpha, (int)Rendering::BlendMode::One);
                break;
            }
        case BlendMode::PerfectVisibility:
            {
                material->SetInt(SrcColor, (int)Rendering::BlendMode::One);
                material->SetInt(DestColor, (int)Rendering::BlendMode::Zero);
                material->SetInt(SrcAlpha, (int)Rendering::BlendMode::Zero);
                material->SetInt(DestAlpha, (int)Rendering::BlendMode::One);
            }
        }
    }
} // namespace Cinema