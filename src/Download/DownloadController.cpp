#include "Download/DownloadController.hpp"
#include "ModConfig.hpp"
#include "Settings/VideoQuality.hpp"
#include "Video/VideoConfig.hpp"
#include "logger.hpp"
#include "assets.hpp"

#include "hollywood/shared/hollywood.hpp"

#include "Python/Python.hpp"
#include "cpython/Python.h"

#include "zip/src/zip.h"

#include <filesystem>
#include <mutex>
#include <thread>
#include <vector>
#include <stop_token>

static const std::string YTDLP_HASH(YTDLP_FILE_HASH);

namespace Cinema
{
    DownloadController::DownloadController()
    {
        downloadWorkerThread = std::jthread(std::bind_front(&DownloadController::DownloadWorkerThread, this));
        downloadWorkerThread.detach();
    }

    bool DownloadController::IsReady()
    {
        return isReady;
    }

    void DownloadController::Setup()
    {
        if(isReady)
        {
            return;
        }

        try
        {
            Python::Setup();
            std::string_view data = IncludedAssets::yt_dlp_zip;
            const std::filesystem::path YTDLP_PATH = Python::SCRIPTS_DIRECTORY / "yt_dlp";
            if(!std::filesystem::exists(YTDLP_PATH))
            {
                std::filesystem::create_directories(YTDLP_PATH);
            }
            int arg = 2;
            if(int res = zip_stream_extract(data.data(), data.size(), YTDLP_PATH.c_str(), nullptr, &arg); res != 0)
            {
                throw std::runtime_error(std::format("Failed to extract ytdlp: {})", zip_strerror(res)));
            }

            Python::QueueCommand("from yt_dlp import _real_main");

            isReady = true;
            INFO("Python setup successful");
        }
        catch(std::exception& e)
        {
            ERROR("Python setup failed: {}", e.what());
        }
    }

    void DownloadController::DownloadWorkerThread()
    {
        auto getNextItem = [this] -> std::optional<std::function<void()>>
        {
            std::lock_guard lock(downloadWorkerMutex);
            if(!downloadWorkerQueue.empty())
            {
                auto val = downloadWorkerQueue.front();
                downloadWorkerQueue.pop();
                return val;
            }
            return std::nullopt;
        };

        while(true)
        {
            if(auto func = getNextItem(); func.has_value())
            {
                func.value()();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

    std::filesystem::path DownloadController::GetTemporaryDownloadFolder()
    {
        std::string uuid(readfile("/proc/sys/kernel/random/uuid"));
        std::filesystem::path videoTempFolder(getDataDir("Cinema"));
        // for some reason it has a newline character at the end
        videoTempFolder = videoTempFolder / "tmp" / uuid.substr(0, uuid.size() - 1);
        return videoTempFolder;
    }

    void DownloadController::StartDownload(std::shared_ptr<VideoConfig> video, VideoQuality::Mode quality)
    {
        video->downloadState = DownloadState::Preparing;
        onDownloadProgress.invoke(video);
        std::string videoUrl;
        if(video->videoUrl.has_value())
        {
            videoUrl = video->videoUrl.value();
        }
        else if(video->videoID.has_value())
        {
            videoUrl = std::string("https://www.youtube.com/watch?v=").append(video->videoID.value());
        }
        else
        {
            ERROR("Video had no valid url!");
            return;
        }
        
        std::lock_guard lock(downloadWorkerMutex);
        downloadWorkerQueue.emplace([video, quality, videoUrl, this]
        {
            namespace fs = std::filesystem;

            auto stdoutWriteHandler = [&](std::string_view data)
            {
                if(data.find("[download]") != std::string::npos)
                {
                    if(data.ends_with(".mp4"))
                    {
                        video->downloadState = DownloadState::DownloadingVideo;
                    }
                    else if(data.ends_with(".m4a"))
                    {
                        video->downloadState = DownloadState::DownloadingAudio;
                    }
                    
                    if(data.find('%') == std::string::npos)
                    {
                        return;
                    }

                    auto percentange = data.substr(11, 5);
                    if(percentange.ends_with('%'))
                        percentange = percentange.substr(0, percentange.size() -1 );

                    video->downloadProgress = std::stof(percentange.data());
                    onDownloadProgress.invoke(video);
                }
            };

            fs::path tempFolder = GetTemporaryDownloadFolder();
            DEBUG("Queueing download command");
            std::string command = std::format("_real_main(['-v', '--no-playlist', '--no-part', '--no-mtime', '--no-cache-dir', '-o', 'video.mp4', '-f', '{}', '-P', '{}', '{}'])",
                VideoQuality::ToYoutubeDLFormat(video, quality),
                tempFolder.c_str(),
                videoUrl
            );

            Python::StandardOutputWriteEvent += stdoutWriteHandler;
            std::future<int> fut = Python::QueueCommand(command);
            fut.wait();
            DEBUG("Python returned code {}", fut.get());
            Python::StandardOutputWriteEvent -= stdoutWriteHandler;
            
            auto files = fs::directory_iterator(tempFolder);
            auto videoFile = (files | std::views::filter([](const auto& e){return e.path().filename().string().ends_with(".mp4");})).begin()->path();
            auto audioFile = (files | std::views::filter([](const auto& e){return e.path().filename().string().ends_with(".m4a");})).begin()->path();
            
            DEBUG("Muxing downloaded files");
            DEBUG("Video: {}", videoFile.c_str());
            DEBUG("Audio: {}", audioFile.c_str());
            
            video->downloadState = DownloadState::Converting;
            onDownloadProgress.invoke(video);
            fs::path muxOutput = tempFolder / "video.mp4";
            fs::path destinationFile = fs::path(video->levelDir.value()) / video->GetVideoFileName(video->levelDir.value());

            Hollywood::MuxFilesSync(videoFile.c_str(), audioFile.c_str(), muxOutput.c_str());
            fs::copy(muxOutput, destinationFile);
            video->downloadState = DownloadState::Downloaded;
            onDownloadFinished.invoke(video);
            INFO("Finished video download");
        });
    }
}