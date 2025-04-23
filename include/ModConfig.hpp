#pragma once
#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ModConfig) {
    CONFIG_VALUE(ytdlpFileHash, std::string, "ytdlpFileHash", "");
    CONFIG_VALUE(enabled, bool, "PluginEnabled", true);
    CONFIG_VALUE(overrideEnvironment, bool, "OverrideEnvironment", true);
    CONFIG_VALUE(enable360Rotation, bool, "Enable360Rotation", true);
    CONFIG_VALUE(curvedScreen, bool, "CurvedScreen", true);
    CONFIG_VALUE(transparencyEnabled, bool, "TransparancyEnabled", true);
    CONFIG_VALUE(colorBlendingEnabled, bool, "ColorBlendingEnabled", true);
    CONFIG_VALUE(coverEnabled, bool, "CoverEnabled", false);
    CONFIG_VALUE(bloomIntensity, int, "BloomIntensity", 100);
    CONFIG_VALUE(cornerRoundness, float, "CornerRoundness", 0);
    CONFIG_VALUE(qualityMode, int, "QualityMode", 720);
    CONFIG_VALUE(forceDisableEnvironmentOverrides, bool, "ForceDisableEnvironmentOverrides", false);
};