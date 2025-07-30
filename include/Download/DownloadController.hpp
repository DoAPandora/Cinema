#pragma once

#include <Video/VideoConfig.hpp>
#include "Settings/VideoQuality.hpp"

#include <logger.hpp>

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

#include <mutex>
#include <queue>
#include <filesystem>

namespace Cinema
{
    class DownloadController
    {
    public:

        DownloadController();
        
        static void Setup();
        static bool IsReady();
        
        UnorderedEventCallback<std::shared_ptr<VideoConfig>> onDownloadProgress;
        UnorderedEventCallback<std::shared_ptr<VideoConfig>> onDownloadFinished;
        
        void StartDownload(std::shared_ptr<VideoConfig> video, VideoQuality::Mode quality);
        
    private:

        static std::filesystem::path GetTemporaryDownloadFolder();

        void DownloadWorkerThread();

        std::jthread downloadWorkerThread;
        std::queue<std::function<void()>> downloadWorkerQueue;
        std::mutex downloadWorkerMutex;

        static inline bool isReady = false;
    };
}