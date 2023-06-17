#include "main.hpp"
#include "Video/VideoLoader.hpp"

#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"

#include <filesystem>

using namespace GlobalNamespace;

std::map<StringW, Cinema::VideoConfig> cachedConfigs;

namespace Cinema::VideoLoader {

    std::string GetLevelPath(IPreviewBeatmapLevel* level)
    {
        auto custom = il2cpp_utils::try_cast<CustomPreviewBeatmapLevel>(level);
        return custom.has_value() ? static_cast<std::string>(custom.value()->customLevelPath) : "";
    }

    std::string GetConfigPath(IPreviewBeatmapLevel* level)
    {
        std::string path = GetLevelPath(level);
        if(!path.empty())
            path += "/cinema-video.json";
        return path;
    }

    Cinema::VideoConfig LoadConfig(IPreviewBeatmapLevel* level)
    {
        Cinema::VideoConfig config;
            std::string path = GetConfigPath(level);
        try {
            ReadFromFile(path, config);
            config.configPath = path;
            cachedConfigs.insert({level->get_levelID(), config});
        } catch(std::exception& err) {
            getLogger().error("An error occurred while loading config from file '%s'\n%s", path.c_str(), err.what());
        }
        return config;
    }

    std::optional<Cinema::VideoConfig> GetConfigForLevel(IPreviewBeatmapLevel* level)
    {
        StringW levelID = level->get_levelID();

        if(!levelID->StartsWith("custom_level"))
            return GetConfigFromBundledConfigs(level);
        
        if(cachedConfigs.contains(levelID))
            return cachedConfigs.at(levelID);
        
        return LoadConfig(level);
    }

    std::optional<Cinema::VideoConfig> GetConfigFromBundledConfigs(IPreviewBeatmapLevel* level)
    {
        return {};
    }

    void SaveVideoConfig(VideoConfig& config)
    {
        WriteToFile(config.configPath, config);
    }

    void DeleteVideo(VideoConfig& config)
    {
        if(std::filesystem::exists(config.get_videoPath()))
            std::filesystem::remove(config.get_videoPath());
        config.downloadState = DownloadState::NotDownloaded;
    }
}