#pragma once
#include "rapidjson-macros/shared/macros.hpp"

#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/GameObject.hpp"

// Unity type serialization
#include "config-utils/shared/config-utils.hpp"

#include <filesystem>

class EnvironmentObject;

namespace Cinema {

    enum class DownloadState {
        Downloaded,
        Downloading,
        NotDownloaded
    };

    DECLARE_JSON_CLASS(EnvironmentModification,
        VALUE(std::string, name);
        VALUE_OPTIONAL(std::string, parentName);
        VALUE_OPTIONAL(std::string, cloneFrom);
        VALUE_OPTIONAL(bool, active);
        VALUE_OPTIONAL(ConfigUtils::Vector3, position);
        VALUE_OPTIONAL(ConfigUtils::Vector3, rotation);
        VALUE_OPTIONAL(ConfigUtils::Vector3, scale);

        UnityEngine::GameObject* gameObject;
        EnvironmentObject* gameObjectClone;
    );

    DECLARE_JSON_CLASS(ColorCorrection,
       VALUE_OPTIONAL(float, brightness);
       VALUE_OPTIONAL(float, contrast);
       VALUE_OPTIONAL(float, saturation);
       VALUE_OPTIONAL(float, hue);
       VALUE_OPTIONAL(float, exposure);
       VALUE_OPTIONAL(float, gamma);
    );

    DECLARE_JSON_CLASS(Vigenette,
        VALUE_OPTIONAL(std::string, type);
        VALUE_OPTIONAL(float, radius);
        VALUE_OPTIONAL(float, softness);
    );

    DECLARE_JSON_CLASS(ScreenConfig,
        VALUE_OPTIONAL(ConfigUtils::Vector3, position);
        VALUE_OPTIONAL(ConfigUtils::Vector3, rotation);
        VALUE_OPTIONAL(ConfigUtils::Vector3, scale);
    );

    DECLARE_JSON_CLASS(UserSettings,
        VALUE_OPTIONAL(bool, customOffset);
        VALUE_OPTIONAL(int, originalOffset);
    )

    DECLARE_JSON_CLASS(VideoConfig,
        VALUE_OPTIONAL(std::string, videoID);
        VALUE_OPTIONAL(std::string, videoUrl);
        VALUE_OPTIONAL(std::string, title);
        VALUE_OPTIONAL(std::string, author);
        VALUE_OPTIONAL(std::string, videoFile);
        VALUE(int, duration); // seconds
        VALUE(int, offset); // milliseconds
        VALUE_OPTIONAL(float, playbackSpeed);
        VALUE_OPTIONAL(bool, loop);
        VALUE_OPTIONAL(float, endVideoAt);
        VALUE_OPTIONAL(bool, configByMapper);
        VALUE_OPTIONAL(bool, bundledConfig);

        VALUE_OPTIONAL(ConfigUtils::Vector3, screenPosition);
        VALUE_OPTIONAL(ConfigUtils::Vector3, screenRotation);
        VALUE_OPTIONAL(float, screenHeight);
        VALUE_OPTIONAL(float, screenCurvature);
        VALUE_OPTIONAL(bool, curveYAxis);
        VALUE_OPTIONAL(int, screenSubsurfaces);
        VALUE_OPTIONAL(float, bloom);
        VALUE_OPTIONAL(bool, disableDefaultModifications);
        VALUE_OPTIONAL(std::string, environmentName);
        VALUE_OPTIONAL(bool, forceEnvironmentModifications);
        VALUE_OPTIONAL(bool, allowCustomPlatforms);
        VALUE_OPTIONAL(bool, mergePropGroups);
        VALUE_OPTIONAL(bool, colorBlending);

        VALUE_OPTIONAL(ColorCorrection, colorCorrection);
        VALUE_OPTIONAL(Vigenette, vigenette);
        VECTOR_OPTIONAL(EnvironmentModification, environment);
        VECTOR_OPTIONAL(ScreenConfig, additionalScreens);

        std::string configPath;

        DownloadState downloadState;

        std::string get_videoPath() {
            return "/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Videos/" + videoFile.value_or(videoID.value() + ".mp4");
        }

        DownloadState UpdateDownloadState() {
            downloadState = std::filesystem::exists(get_videoPath()) ? DownloadState::Downloaded : DownloadState::NotDownloaded;
            return downloadState;
        }

        float get_offsetInSeconds() {
            return offset / 1000.0f;
        }

        bool get_isPlayable() {
            return true;
        }

        DESERIALIZE_ACTION(0,
            self->UpdateDownloadState();
        )
    );
}