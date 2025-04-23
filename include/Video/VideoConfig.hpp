#pragma once
#include "rapidjson-macros/shared/macros.hpp"

#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/GameObject.hpp"

// Unity type serialization
#include "config-utils/shared/config-utils.hpp"

#include <filesystem>

#include <functional>

class EnvironmentObject;

namespace Cinema {

    enum class DownloadState {
        NotDownloaded,
        Preparing,
        Downloading,
        DownloadingVideo,
        DownloadingAudio,
        Converting,
        Downloaded,
        Cancelled
    };

    DECLARE_JSON_STRUCT(VideoConfig) {

    public:

        DECLARE_JSON_STRUCT(EnvironmentModification) {
            VALUE(std::string, name);
            VALUE_OPTIONAL(std::string, parentName);
            VALUE_OPTIONAL(std::string, cloneFrom);
            VALUE_OPTIONAL(bool, active);
            VALUE_OPTIONAL(ConfigUtils::Vector3, position);
            VALUE_OPTIONAL(ConfigUtils::Vector3, rotation);
            VALUE_OPTIONAL(ConfigUtils::Vector3, scale);

            UnityEngine::GameObject* gameObject;
            EnvironmentObject* gameObjectClone;
        };

        DECLARE_JSON_STRUCT(ColorCorrection) {
            VALUE_OPTIONAL(float, brightness);
            VALUE_OPTIONAL(float, contrast);
            VALUE_OPTIONAL(float, saturation);
            VALUE_OPTIONAL(float, hue);
            VALUE_OPTIONAL(float, exposure);
            VALUE_OPTIONAL(float, gamma);
        };

        DECLARE_JSON_STRUCT(Vigenette) {
            VALUE_OPTIONAL(std::string, type);
            VALUE_OPTIONAL(float, radius);
            VALUE_OPTIONAL(float, softness);
        };

        DECLARE_JSON_STRUCT(ScreenConfig) {
            VALUE_OPTIONAL(ConfigUtils::Vector3, position);
            VALUE_OPTIONAL(ConfigUtils::Vector3, rotation);
            VALUE_OPTIONAL(ConfigUtils::Vector3, scale);
        };

        DECLARE_JSON_STRUCT(UserSettings) {
            VALUE_OPTIONAL(bool, customOffset);
            VALUE_OPTIONAL(int, originalOffset);
        };

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

        VALUE_OPTIONAL(bool, transparency);

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
        VALUE_OPTIONAL(UserSettings, userSettings);



    public:

        DownloadState downloadState;
        std::string errorMessage;
        bool backCompat;
        bool needsToSave;
        bool playbackDisabledByMissingSuggestion;
        float downloadProgress;
        std::optional<std::string> levelDir;


        bool get_IsWIPLevel() const;
        __declspec(property(get=get_IsWIPLevel)) bool IsWIPLevel;

        bool get_IsOfficialConfig() const;
        __declspec(property(get=get_IsOfficialConfig)) bool IsOfficialConfig;

        bool get_TransparencyEnabled() const;
        __declspec(property(get=get_TransparencyEnabled)) bool TransparencyEnabled;

        bool get_IsDownloading() const;
        __declspec(property(get=get_IsDownloading)) bool IsDownloading;

        std::optional<std::string> get_VideoPath();
        __declspec(property(get=get_VideoPath)) std::optional<std::string> VideoPath;

        std::optional<std::string> get_ConfigPath() const;
        __declspec(property(get=get_ConfigPath)) std::optional<std::string> ConfigPath;

        bool get_isPlayable() const;

        std::string GetVideoFileName(std::string levelPath) const;

        std::string ToString() const;

        float GetOffsetInSec() const;

        DownloadState UpdateDownloadState();

    };

    DECLARE_JSON_STRUCT(BundledConfig) {
       VALUE(std::string, levelID);
       VALUE(Cinema::VideoConfig, config);
    };
}