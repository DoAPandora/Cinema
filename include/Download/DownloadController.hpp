#pragma once

#include <Video/VideoConfig.hpp>

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
    public:

        UnorderedEventCallback<std::shared_ptr<VideoConfig>> onDownloadProgress;
        UnorderedEventCallback<std::shared_ptr<VideoConfig>> onDownloadFinished;

        void StartDownload(std::shared_ptr<VideoConfig> video);
        void CancelDownload(std::shared_ptr<VideoConfig> video);

    private:
        void DownloadVideoThread(std::filesystem::path tempDir, std::shared_ptr<VideoConfig> video, std::stop_token stopToken);
    };
}