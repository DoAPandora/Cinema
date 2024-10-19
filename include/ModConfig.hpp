#pragma once
#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(ModConfig,
    CONFIG_VALUE(enabled, bool, "enabled", true);
    CONFIG_VALUE(ytdlpFileHash, std::string, "ytdlpFileHash", "");
)