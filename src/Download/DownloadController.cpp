#include "Download/DownloadController.hpp"
#include "Video/VideoConfig.hpp"
#include "logger.hpp"

#include "pythonlib/shared/Python.hpp"
#include "pythonlib/shared//Utils/StringUtils.hpp"
#include <filesystem>
#include <mutex>
#include <vector>

static const std::filesystem::path DOWNLOAD_FOLDER("/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Videos");
static constexpr std::string_view BASE_DOWNLOAD_COMMAND("--no-cache-dir -o %(id)s.%(ext)s -P /sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Videos ");

namespace Cinema
{
    void DownloadController::DownloadVideoThread(std::shared_ptr<VideoConfig> video, const std::function<void(std::shared_ptr<VideoConfig>, bool)> onFinished, System::Threading::CancellationToken cancellationToken)
    {
        INFO("Downloading video {}", video->videoID);
        std::function<void(int, char*)> eventHandler = [video](int type, char* data)
        {
            switch(type)
            {
            case 0:
                {
                    std::string_view dataString(data);
                    video->downloadState = DownloadState::Downloading;
                    if(dataString.find("[download]", 0) != -1)
                    {
                        auto pos = dataString.find("%", 0);
                        if(pos != -1 && pos > 5)
                        {
                            auto percentange = dataString.substr(pos - 5, 5);
                            if(percentange.find("]", 0) == 0)
                                percentange = percentange.substr(1);
                            video->downloadProgress = std::stof(percentange.data());
                        }
                    }
                }
                break;
            case 1:
                break;
            }
        };
        Python::PythonWriteEvent += eventHandler;
        Python::PyRun_SimpleString("from yt_dlp.__init__ import _real_main");

        std::string command{"_real_main([\"--no-cache-dir\", \"-v\", \"-o\", \"%(id)s.%(ext)s\", \"-P\", \""};
        std::filesystem::path videoOutputDir = video->levelDir.value();
        command.append(videoOutputDir)
            .append("\",\"").append(video->videoID.value()).append("\"])");
        DEBUG("Running command {}", command);

        int status = Python::PyRun_SimpleString(command.c_str());
        bool error = status != 0;
        Python::PythonWriteEvent -= eventHandler;

        DEBUG("Python process returned result {}", status);

        {
            std::lock_guard lock(currentDownloadsMutex);
            // remove current download from the downloads list
            std::erase_if(currentDownloads, [video](const std::pair<std::shared_ptr<VideoConfig>, System::Threading::CancellationTokenSource*>& downloadInfo)
                { return downloadInfo.first == video; });
        }

        std::string id = video->videoID.value();
        const auto path = DOWNLOAD_FOLDER / (id + ".mp4");
        if(cancellationToken.IsCancellationRequested)
        {
            DEBUG("Video {} download was cancelled!", video->videoID);
            if(std::filesystem::exists(path))
            {
                std::filesystem::remove(path);
            }
            video->downloadState = DownloadState::Cancelled;
            onFinished(video, false);
            return;
        }

        if(error)
        {
            DEBUG("Video {} download errored!", video->videoID);
            video->downloadState = DownloadState::NotDownloaded;
            if(std::filesystem::exists(path))
            {
                std::filesystem::remove(path);
            }
            onFinished(video, false);
            return;
        }

        std::filesystem::rename(videoOutputDir / (video->videoID.value() + ".mp4"), videoOutputDir / video->GetVideoFileName(video->levelDir.value()));

        onFinished(video, true);
    }

    void DownloadController::CancelDownload(std::shared_ptr<VideoConfig> video)
    {
        std::lock_guard lock(currentDownloadsMutex);
        auto downloadInfo = std::find_if(currentDownloads.begin(), currentDownloads.end(), [video](const std::pair<std::shared_ptr<VideoConfig>, System::Threading::CancellationTokenSource*>& downloadInfo)
            { return downloadInfo.first == video; });
        if(downloadInfo == currentDownloads.end())
        {
            DEBUG("Attempted to cancel download of video {}, but download did not exist!", video->videoID);
            return;
        }

        downloadInfo->second->Cancel();
    }
}