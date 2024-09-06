#pragma once

#include <Video/VideoConfig.hpp>

#include <System/Threading/CancellationToken.hpp>
#include <System/Threading/CancellationTokenSource.hpp>

#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace Cinema
{
    class DownloadController
    {
        std::mutex currentDownloadsMutex;
        std::vector<std::pair<std::shared_ptr<VideoConfig>, System::Threading::CancellationTokenSource*>> currentDownloads;

    public:
        inline void StartDownload(std::shared_ptr<VideoConfig> video, const std::function<void(std::shared_ptr<VideoConfig>)>& statusUpdate, const std::function<void(std::shared_ptr<VideoConfig>, bool)>& onFinished)
        {
            if(!video)
            {
                return onFinished(nullptr, false);
            }

            video->downloadState = DownloadState::Preparing;
            auto source = System::Threading::CancellationTokenSource::New_ctor();
            il2cpp_utils::il2cpp_aware_thread(&DownloadController::DownloadVideoThread, 
                std::forward<DownloadController*>(this), std::forward<std::shared_ptr<VideoConfig>>(video), std::forward<const std::function<void(std::shared_ptr<VideoConfig>)>>(statusUpdate), std::forward<const std::function<void(std::shared_ptr<VideoConfig>, bool)>>(onFinished), std::forward<System::Threading::CancellationToken>(source->get_Token())
            ).detach();
            std::lock_guard lock(currentDownloadsMutex);
            currentDownloads.push_back({video, source});
        }

        void CancelDownload(std::shared_ptr<VideoConfig> video);

    private:
        void DownloadVideoThread(std::shared_ptr<VideoConfig> video, const std::function<void(std::shared_ptr<VideoConfig>)> statusUpdate, const std::function<void(std::shared_ptr<VideoConfig>, bool)> onFinished, System::Threading::CancellationToken cancellationToken);
        
    };
}