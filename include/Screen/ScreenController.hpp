#pragma once

#include "custom-types/shared/macros.hpp"

#include "System/Object.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/MaterialPropertyBlock.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/RenderTexture.hpp"
#include "UnityEngine/Shader.hpp"

#include "Video/VideoConfig.hpp"
#include "Placement.hpp"

#include <optional>

DECLARE_CLASS_CODEGEN(Cinema, ScreenController, System::Object,

public:
    enum class BlendMode {
        SoftAdditive,
        PerfectVisibility
    };

    DECLARE_INSTANCE_FIELD_DEFAULT(ListW<UnityW<UnityEngine::GameObject>>, screens, ListW<UnityW<UnityEngine::GameObject>>::New());
    DECLARE_INSTANCE_FIELD(UnityEngine::MaterialPropertyBlock*, materialPropertyBlock);

private:

        inline static int Brightness = UnityEngine::Shader::PropertyToID("_Brightness");
        inline static int Contrast = UnityEngine::Shader::PropertyToID("_Contrast");
        inline static int Saturation = UnityEngine::Shader::PropertyToID("_Saturation");
        inline static int Hue = UnityEngine::Shader::PropertyToID("_Hue");
        inline static int Gamma = UnityEngine::Shader::PropertyToID("_Gamma");
        inline static int Exposure = UnityEngine::Shader::PropertyToID("_Exposure");
        inline static int VignetteRadius = UnityEngine::Shader::PropertyToID("_VignetteRadius");
        inline static int VignetteSoftness = UnityEngine::Shader::PropertyToID("_VignetteSoftness");
        inline static int VignetteElliptical = UnityEngine::Shader::PropertyToID("_VignetteOval");
        inline static int SrcColor = UnityEngine::Shader::PropertyToID("_SrcColor");
        inline static int DestColor = UnityEngine::Shader::PropertyToID("_DestColor");
        inline static int SrcAlpha = UnityEngine::Shader::PropertyToID("_SrcAlpha");
        inline static int DestAlpha = UnityEngine::Shader::PropertyToID("_DestAlpha");

        inline static ConstString BODY_SHADER_NAME = "Custom/OpaqueNeonLight";

public:

    DECLARE_INSTANCE_METHOD(void, CreateScreen, UnityEngine::Transform* parent);
    DECLARE_INSTANCE_METHOD(void, OnGameSceneLoadedFresh);
    DECLARE_INSTANCE_METHOD(void, SetScreensActive, bool active);
    DECLARE_INSTANCE_METHOD(void, SetScreenBodiesActive, bool active);
    DECLARE_INSTANCE_METHOD(UnityEngine::Renderer*, GetRenderer);
    DECLARE_INSTANCE_METHOD(UnityEngine::RenderTexture*, CreateRenderTexture);
    DECLARE_INSTANCE_METHOD(void, RegenerateScreenSurfaces);
    DECLARE_INSTANCE_METHOD(void, SetAspectRatio, float ratio);
    DECLARE_INSTANCE_METHOD(void, SetSoftParent, UnityEngine::Transform* parent);
    DECLARE_INSTANCE_METHOD(void, EnableColorBlending, bool enable);

    DECLARE_CTOR(ctor);

public:

    static void CreateScreenBody(UnityEngine::Component* parent);
    static void AssignBodyMaterial(UnityEngine::Renderer* bodyRenderer);

    void SetPlacement(const Placement& placement);
    void SetPlacement(UnityEngine::Vector3 position, UnityEngine::Vector3 rotation, float width, float height, std::optional<float> curvatureDegrees = std::nullopt, std::optional<int> subsurfaces = std::nullopt, std::optional<bool> curveYAxis = false);
    void InitializeSurfaces(float width, float height, float distance, std::optional<float> curvatureDegrees, std::optional<int> subsurfaces, std::optional<bool> curveYAxis);
    void SetBloomIntensity(std::optional<float> bloomIntensity);
    void SetShaderParameters(std::shared_ptr<VideoConfig> config);
    void SetVigenette(std::optional<VideoConfig::Vigenette> vigenette = std::nullopt, UnityEngine::MaterialPropertyBlock* materialPropertyBlock = nullptr);
    void SetShaderFloat(int nameID, std::optional<float> value, float min, float max, float defaultValue);
    void SetBlendMode(BlendMode blendMode, UnityEngine::Material* material);
)