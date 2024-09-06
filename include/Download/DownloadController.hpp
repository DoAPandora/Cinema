#pragma once

#include <Video/VideoConfig.hpp>

#include <logger.hpp>

#include <functional>
#include <mutex>
#include <utility>
#include <vector>
#include <thread>
#include <stop_token>

namespace Cinema
{
    class DownloadController
    {
        std::mutex currentDownloadsMutex;
        std::vector<std::pair<std::shared_ptr<VideoConfig>, std::stop_source>> currentDownloads;

    public:

        inline void StartDownload(std::shared_ptr<VideoConfig> video, const std::function<void(std::shared_ptr<VideoConfig>)>& statusUpdate, const std::function<void(std::shared_ptr<VideoConfig>, bool)>& onFinished)
        {
            if(!video)
            {
                return onFinished(nullptr, false);
            }

            video->downloadState = DownloadState::Preparing;
            std::stop_source stopSource;
            std::jthread(std::bind_front(&DownloadController::DownloadVideoThread, this),
                stopSource.get_token(), video, std::forward<const std::function<void(std::shared_ptr<VideoConfig>)>>(statusUpdate), std::forward<const std::function<void(std::shared_ptr<VideoConfig>, bool)>>(onFinished)
            ).detach();
            std::lock_guard lock(currentDownloadsMutex);
            currentDownloads.emplace_back(video, stopSource);
        }

        void CancelDownload(std::shared_ptr<VideoConfig> video);

    private:
        void DownloadVideoThread(std::stop_token stopToken, std::shared_ptr<VideoConfig> video, const std::function<void(std::shared_ptr<VideoConfig>)> statusUpdate, const std::function<void(std::shared_ptr<VideoConfig>, bool)> onFinished);
    };
}