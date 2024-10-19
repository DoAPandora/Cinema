#include "Download/DownloadController.hpp"
#include "Video/VideoConfig.hpp"
#include "logger.hpp"

#include "pythonlib/shared/Python.hpp"

#include <filesystem>
#include <mutex>
#include <vector>
#include <stop_token>

namespace Cinema
{
    void DownloadController::StartDownload(std::shared_ptr<VideoConfig> video)
    {
        video->downloadState = DownloadState::Preparing;

        std::string uuid(readfile("/proc/sys/kernel/random/uuid"));
        std::filesystem::path videoTempFolder(getDataDir("Cinema"));
        // for some reason it has a newline character at the end
        videoTempFolder = videoTempFolder / "tmp" / uuid.substr(0, uuid.size() - 1);
        std::lock_guard lock(currentDownloadsMutex);
        std::stop_source stopSource;
        currentDownloads.emplace_back(video, stopSource);

        std::jthread(std::bind_front(&DownloadController::DownloadVideoThread, this),
            std::move(videoTempFolder), std::move(video), stopSource.get_token()
        ).detach();
    }

    void DownloadController::DownloadVideoThread(std::filesystem::path tempDir, std::shared_ptr<VideoConfig> video, std::stop_token stopToken)
    {
        INFO("Downloading video {}", video->videoID);

        if(!std::filesystem::exists(tempDir))
        {
            std::filesystem::create_directories(tempDir);
        }

        std::function<void(int, char*)> eventHandler = [&](int type, char* data)
        {
            if(stopToken.stop_requested())
            {
                return;
            }
            switch(type)
            {
            case 0:
                {
                    std::string_view dataString(data);
                    if(dataString.find("[download]") != std::string::npos && dataString.find('%') != std::string::npos)
                    {
                        video->downloadState = DownloadState::Downloading;
                        
                        auto percentange = dataString.substr(11, 5);
                        if(percentange.ends_with('%'))
                            percentange = percentange.substr(0, percentange.size() -1 );

                        video->downloadProgress = std::stof(percentange.data());
                        onDownloadProgress.invoke(video);
                    }
                }
                break;
            case 1:
                break;
            }
        };
        Python::PythonWriteEvent += eventHandler;
        Python::PyRun_SimpleString("from yt_dlp.__init__ import _real_main");

                                                        // vp9 1080p -> vp8 1080p -> h264 1080p -> ...720p -> ...480p
        std::string command(fmt::format("_real_main(['-v', '-f', '248/616/170/137/216/247/169/136/244/168/135', '-o', 'video.mp4', '-P', '{}', '{}'])", tempDir.c_str(), video->videoID.value()));
        DEBUG("Running command {}", command);

        Python::PythonWriteEvent += eventHandler;
        int status = Python::PyRun_SimpleString(command.c_str());
        bool error = status != 0;
        DEBUG("Python process returned result {}", status);
        Python::PythonWriteEvent -= eventHandler;

        {
            std::lock_guard lock(currentDownloadsMutex);
            // remove current download from the downloads list
            std::erase_if(currentDownloads, [&](const std::pair<std::shared_ptr<VideoConfig>, std::stop_source>& downloadInfo)
                { return downloadInfo.first == video; });
        }

        if(error)
        {
            ERROR("An error occured while downloading video");
        } else if(stopToken.stop_requested())
        {
            DEBUG("Stop was requested on download");
        }


        if(!error && !stopToken.stop_requested() && std::filesystem::exists(tempDir / "video.mp4"))
        {
            std::filesystem::rename(tempDir / "video.mp4", std::filesystem::path(video->levelDir.value()) / video->GetVideoFileName(video->levelDir.value()));
        }
        std::filesystem::remove(tempDir);

        video->UpdateDownloadState();
        if(!error && !stopToken.stop_requested())
        {
            onDownloadFinished.invoke(video);
        }
    }

    void DownloadController::CancelDownload(std::shared_ptr<VideoConfig> video)
    {
        std::lock_guard lock(currentDownloadsMutex);
        auto downloadInfo = std::find_if(currentDownloads.begin(), currentDownloads.end(), [video](const std::pair<std::shared_ptr<VideoConfig>, std::stop_source>& downloadInfo)
            { return downloadInfo.first == video; });
        if(downloadInfo == currentDownloads.end())
        {
            DEBUG("Attempted to cancel download of video {}, but download did not exist!", video->videoID);
            return;
        }

        downloadInfo->second.request_stop();
        DEBUG("Requested stop for download {}", downloadInfo->first->videoID);
        currentDownloads.erase(downloadInfo);
    }
}