#pragma once

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "Video/VideoConfig.hpp"

namespace Cinema::VideoLoader {

    std::string GetLevelPath(GlobalNamespace::IPreviewBeatmapLevel* level);

    std::string GetConfigPath(GlobalNamespace::IPreviewBeatmapLevel* level);

    Cinema::VideoConfig LoadConfig(GlobalNamespace::IPreviewBeatmapLevel* level);

    std::optional<VideoConfig> GetConfigForLevel(GlobalNamespace::IPreviewBeatmapLevel* level);

    std::optional<Cinema::VideoConfig> GetConfigFromBundledConfigs(GlobalNamespace::IPreviewBeatmapLevel* level);
    
    void SaveVideoConfig(VideoConfig& config);

    void DeleteVideo(VideoConfig& config);
}