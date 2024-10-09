#include "Screen/Placement.hpp"
#include "Util/Util.hpp"

namespace Cinema
{
    Placement Placement::SoloGameplayPlacement {
        UnityEngine::Vector3(0, 12.4f, 67.8f),
        UnityEngine::Vector3(-7, 0, 0),
        25
    };

    Placement Placement::MultiplayerPlacement {
        UnityEngine::Vector3(0, 5, 67),
        UnityEngine::Vector3(-5, 0, 0),
        17
    };

    Placement Placement::MenuPlacement {
        UnityEngine::Vector3(0, 4, 16),
        UnityEngine::Vector3(0, 0, 0), 
        8.0f
    };

    Placement Placement::CoverPlacement {
        UnityEngine::Vector3(0, 5.9, 75),
        UnityEngine::Vector3(-8, 0, 0),
        12
    };

    Placement Placement::CreatePlacementForConfig(std::shared_ptr<VideoConfig> config, Scene scene, float aspectRatio)
    {
        Placement defaultPlacement = GetDefaultPlacementForScene(scene);
        if(scene == Scene::MultiplayerGameplay || !config)
        {
            return defaultPlacement;
        }

        Placement placement {
            config->screenPosition.value_or(defaultPlacement.position),
            config->screenRotation.value_or(defaultPlacement.rotation),
            config->screenHeight.value_or(defaultPlacement.height),
        };

        placement.width = placement.height * aspectRatio;
        placement.curvature = config->screenCurvature.has_value() ? config->screenCurvature : defaultPlacement.curvature;
        placement.subsurfaces = config->screenSubsurfaces;
        placement.curveYAxis = config->curveYAxis;

        return placement;
    }

    Placement Placement::GetDefaultPlacementForScene(Scene scene)
    {
        switch (scene)
        {
            case Scene::SoloGameplay: return GetDefaultEnvironmentPlacement().value_or(SoloGameplayPlacement);
            case Scene::MultiplayerGameplay: return MultiplayerPlacement;
            case Scene::Menu: return MenuPlacement;
            default: return SoloGameplayPlacement;
        }
    }

    std::optional<Placement> Placement::GetDefaultEnvironmentPlacement()
    {
        std::string name = Util::GetEnvironmentName();
        
        using UnityEngine::Vector3;
        if(name == "LinkinParkEnvironment") return Placement(Vector3(0, 6.2f, 52.7f), Vector3::get_zero(), 16, std::nullopt, 0);
        if(name == "BTSEnvironment") return Placement(Vector3(0, 12.4, 80), Vector3(-7, 0, 0), 25);
        if(name == "OriginsEnvironment") return Placement(Vector3(0, 12.4, 66), Vector3(-7, 0, 0), 25);
        if(name == "KaleidoscopeEnvironment") return Placement(Vector3(0, -0.5, 35), Vector3::get_zero(), 12);
        if(name == "InterscopeEnvironment") return Placement(Vector3(0, 6.3, 37), Vector3::get_zero(), 12.5);
        if(name == "CrabRaveEnvironment") return Placement(Vector3(0, 5.46, 40), Vector3(-5, 0, 0), 13);
        if(name == "MonstercatEnvironment") return Placement(Vector3(0, 5.46, 40), Vector3(-5, 0, 0), 13);
        if(name == "SkrillexEnvironment") return Placement(Vector3(0, 1.5, 40), Vector3::get_zero(), 12);
        if(name == "WeaveEnvironment") return Placement(Vector3(0, 1.5, 21), Vector3::get_zero(), 4.3, std::nullopt, 0);
        if(name == "PyroEnvironment") return Placement(Vector3(0, 12, 60), Vector3::get_zero(), 24, std::nullopt, 0);
        if(name == "EDMEnvironment") return Placement(Vector3(0, 1.5, 25), Vector3::get_zero(), 8);
        if(name == "LizzoEnvironment") return Placement(Vector3(0, 8, 63), Vector3::get_zero(), 16);
        if(name == "Dragons2Environment") return Placement(Vector3(0, 5.8f, 67), Vector3::get_zero(), 33);
        
        return std::nullopt;
    }
}