#include "main.hpp"
#include "VideoManager.hpp"
#include "json.hpp"
#include "VideoDownloader.hpp"

#include "pinkcore/shared/RequirementAPI.hpp"

#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include <filesystem>

namespace Cinema::VideoManager {
    const std::string modDataDir = "/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/";
    JSON::CinemaInfo currentLevelInfo;
    std::string currentLevelPath;
    bool shouldCreateScreen;

    void DidSelectLevel(bool isCustom, std::string levelPath)
    {
        if(!isCustom)
        {
            shouldCreateScreen = false;
            return;
        }

        std::string path = levelPath + "/cinema-video.json";
        if(!std::filesystem::exists(path))
        {
            shouldCreateScreen = false;
            return;
        }

        getLogger().info("Found cinema data on level");
        shouldCreateScreen = true;
        currentLevelPath = levelPath;

        ReadFromFile(path, currentLevelInfo);
        if(currentLevelInfo.videoFile.empty())
            currentLevelInfo.videoFile = currentLevelInfo.title + ".mp4";
    }

    void DownloadCurrentVideo()
    {
        if(std::filesystem::exists(modDataDir + currentLevelInfo.videoFile))
        {
            getLogger().info("Song already downloaded, skipping");
            return;
        }

        PinkCore::RequirementAPI::DisablePlayButton(getModInfo());
        std::thread([]()
        {
            Cinema::DownloadVideo("https://youtu.be/" + currentLevelInfo.videoId, "/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema", [](float percentage)
            {
                getLogger().info("Percentage: %f", percentage);
            },
            [](bool success)
            {
                getLogger().info("Finished downloading video");
                QuestUI::MainThreadScheduler::Schedule([]()
                {
                    std::filesystem::rename(modDataDir + currentLevelInfo.videoId + ".mp4", GetCurrentVideoPath());
                    PinkCore::RequirementAPI::EnablePlayButton(getModInfo());
                });
            });
        }).detach();
    }

    std::string GetCurrentVideoPath()
    {
        return modDataDir + currentLevelInfo.videoFile;
    }

    bool GetShouldCreateScreen()
    {
        return shouldCreateScreen;
    }
}