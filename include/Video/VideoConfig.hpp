#pragma once
#include "rapidjson-macros/shared/macros.hpp"

#include <filesystem>

namespace Cinema {

    enum class DownloadState {
        Downloaded,
        Downloading,
        NotDownloaded
    };

    DECLARE_JSON_CLASS(Vector3,
        VALUE_DEFAULT(float, x, 0);
        VALUE_DEFAULT(float, y, 0);
        VALUE_DEFAULT(float, z, 0);
        CONVERSION(Vector3,
            x = round(other.x); y = round(other.y); z = round(other.z);,
            (x, y, z)
        )
        Vector3() = default;
        Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    )

    DECLARE_JSON_CLASS(EnvironmentModification,
        VALUE(std::string, name);
        VALUE_OPTIONAL(std::string, parentName);
        VALUE_OPTIONAL(std::string, cloneFrom);
        VALUE_OPTIONAL(bool, active);
        VALUE_OPTIONAL(Vector3, position);
        VALUE_OPTIONAL(Vector3, rotation);
        VALUE_OPTIONAL(Vector3, scale);
    );

    DECLARE_JSON_CLASS(ColorCorrection,
        VALUE_DEFAULT(float, brightness, 1.0f);
        VALUE_DEFAULT(float, contrast, 1.0f);
        VALUE_DEFAULT(float, saturation, 1.0f);
        VALUE_DEFAULT(float, exposure, 1.0f);
        VALUE_DEFAULT(float, gamma, 1.0f);
        VALUE_DEFAULT(float, hue, 0.0f);
    );

    DECLARE_JSON_CLASS(Vigenette,
        VALUE_DEFAULT(std::string, type, "rectangular");
        VALUE_DEFAULT(float, radius, 1.0f);
        VALUE_DEFAULT(float, softness, 0.005f);
    );

    DECLARE_JSON_CLASS(ScreenConfig,
        VALUE_OPTIONAL(Vector3, position);
        VALUE_OPTIONAL(Vector3, rotation);
        VALUE_OPTIONAL(Vector3, scale);
    );

    DECLARE_JSON_CLASS(UserSettings,
        VALUE_OPTIONAL(bool, customOffset);
        VALUE_OPTIONAL(int, originalOffset);
    )

    DECLARE_JSON_CLASS(VideoConfig,
        VALUE_OPTIONAL(std::string, videoID);
        VALUE_DEFAULT(std::string, title, "Unknown Title");
        VALUE_DEFAULT(std::string, author, "Unknown Author");
        VALUE_OPTIONAL(std::string, videoFile);
        VALUE_DEFAULT(int, duration, 0);
        VALUE_DEFAULT(int, offset, 0);
        VALUE_DEFAULT(float, playbackSpeed, 1);
        VALUE_DEFAULT(bool, loop, false);
        VALUE_OPTIONAL(float, endVideoAt);
        VALUE_DEFAULT(bool, configByMapper, false);
        VALUE_OPTIONAL(bool, bundledConfig);

        VALUE_DEFAULT(Vector3, screenRotation, Vector3(-8.0f, 0.0f, 0.0f));
        VALUE_DEFAULT(Vector3, screenPosition, Vector3(0.0f, 12.4f, 67.8f));
        VALUE_DEFAULT(float, screenHeight, 25);
        VALUE_OPTIONAL(float, screenCurvature);
        VALUE_OPTIONAL(bool, curveYAxis);
        VALUE_OPTIONAL(int, screenSubsurfaces);
        VALUE_DEFAULT(float, bloom, 1.0f);
        VALUE_DEFAULT(bool, disableDefaultModifications, false);
        VALUE_OPTIONAL(std::string, environmentName);

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