#pragma once

#include <Video/VideoConfig.hpp>
#include "Settings/VideoQuality.hpp"

#include <logger.hpp>

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

#include <mutex>
#include <stop_token>
#include <utility>
#include <vector>
#include <filesystem>

namespace Cinema
{
    class DownloadController
    {
        std::mutex currentDownloadsMutex;
        std::vector<std::pair<std::shared_ptr<VideoConfig>, std::stop_source>> currentDownloads;

        static inline bool isReady = false;
    public:

        static void Setup();
        static bool IsReady();

        UnorderedEventCallback<std::shared_ptr<VideoConfig>> onDownloadProgress;
        UnorderedEventCallback<std::shared_ptr<VideoConfig>> onDownloadFinished;

        void StartDownload(std::shared_ptr<VideoConfig> video, VideoQuality::Mode quality);
        void CancelDownload(std::shared_ptr<VideoConfig> video);

    private:
        void DownloadVideoThread(std::filesystem::path tempDir, std::shared_ptr<VideoConfig> video, VideoQuality::Mode quality, std::stop_token stopToken);
    };
}