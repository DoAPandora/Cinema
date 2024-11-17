#include "Download/DownloadController.hpp"
#include "ModConfig.hpp"
#include "Settings/VideoQuality.hpp"
#include "Video/VideoConfig.hpp"
#include "logger.hpp"
#include "assets.hpp"

#include "hollywood/shared/hollywood.hpp"

#include "pythonlib/shared/Python.hpp"
#include "pythonlib/shared/Utils/FileUtils.hpp"

#include <filesystem>
#include <mutex>
#include <thread>
#include <vector>
#include <stop_token>

using std::filesystem::path;

static const std::string YTDLP_HASH(YTDLP_FILE_HASH);

namespace Cinema
{
    bool DownloadController::IsReady()
    {
        return isReady;
    }

    void DownloadController::Setup()
    {
        std::jthread([](){
            const path ytdlp = path(FileUtils::getScriptsPath()) / "yt_dlp";
            if(getModConfig().ytdlpFileHash.GetValue() != YTDLP_HASH)
            {
                if(direxists(ytdlp.c_str()))
                {
                    DEBUG("Current ytdlp was an old version, removing old version!");
                    std::filesystem::remove_all(ytdlp);
                } else
                {
                    DEBUG("No ytdlp install was found, installing now!");
                }

                FileUtils::ExtractZip(IncludedAssets::ytdlp_zip, ytdlp.c_str());
                getModConfig().ytdlpFileHash.SetValue(YTDLP_HASH);
            } else 
            {
                DEBUG("Current install of ytdlp was already up to date!");
            }

            Python::PyRun_SimpleString("from yt_dlp.__init__ import _real_main");
            DEBUG("Successfully set up ytdlp");
            isReady = true;
        }).detach();
    }

    void DownloadController::StartDownload(std::shared_ptr<VideoConfig> video, VideoQuality::Mode quality)
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
            std::move(videoTempFolder), std::move(video), quality, stopSource.get_token()
        ).detach();
    }

    void DownloadController::DownloadVideoThread(std::filesystem::path tempDir, std::shared_ptr<VideoConfig> video, VideoQuality::Mode quality, std::stop_token stopToken)
    {
        if(!IsReady())
        {
            DEBUG("Python was not ready, waiting");
            while(!isReady)
            {
                std::this_thread::yield();
            }
        }
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
                    if(dataString.find("[download]") != std::string::npos)
                    {
                        if(dataString.ends_with(".mp4"))
                        {
                            video->downloadState = DownloadState::DownloadingVideo;
                        }
                        else if(dataString.ends_with(".m4a"))
                        {
                            video->downloadState = DownloadState::DownloadingAudio;
                        }
                        
                        if(dataString.find('%') == std::string::npos)
                        {
                            break;
                        }

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
            ERROR("Video config has no URL or ID!");
            std::lock_guard lock(currentDownloadsMutex);
            // remove current download from the downloads list
            std::erase_if(currentDownloads, [&](const std::pair<std::shared_ptr<VideoConfig>, std::stop_source>& downloadInfo)
                { return downloadInfo.first == video; });
            video->downloadState = DownloadState::NotDownloaded;
            onDownloadFinished.invoke(video);
            return;
            // set error ?
        }
        
        std::string videoFormat = VideoQuality::ToYoutubeDLFormat(video, quality);
        videoFormat = videoFormat.size() > 0 ? fmt::format("'-f', '{}',", videoFormat) : "";

        video->downloadState = DownloadState::Downloading;
        std::string command(fmt::format("_real_main(['-v', '--no-playlist', '--no-part', '--no-mtime', {} '-o', 'video.mp4', '-P', '{}', '{}'])", videoFormat, tempDir.c_str(), videoUrl));
        DEBUG("Running command {}", command);

        Python::PythonWriteEvent += eventHandler;
        int status = Python::PyRun_SimpleString(command.c_str());
        bool error = status != 0;
        DEBUG("Python process returned result {}", status);
        Python::PythonWriteEvent -= eventHandler;


        if(error)
        {
            ERROR("An error occured while downloading video");
        } else if(stopToken.stop_requested())
        {
            DEBUG("Stop was requested on download");
        }


        if(!error && !stopToken.stop_requested())
        {
            using namespace std::filesystem;

            video->downloadState = DownloadState::Converting;
            onDownloadProgress.invoke(video);

            std::vector<directory_entry> files(directory_iterator(tempDir), {});
            auto videoFile = std::find_if(files.cbegin(), files.cend(), [](auto f){return f.path().filename().string().ends_with(".mp4");});
            auto audioFile = std::find_if(files.cbegin(), files.cend(), [](auto f){return f.path().filename().string().ends_with(".m4a");});
            bool ok = true;
            if(videoFile == files.cend())
            {
                ERROR("Could not find downloaded video file!");
                ok = false;
            }
            if(audioFile == files.cend())
            {
                ERROR("Could not find downloaded video file!");
                ok = false;
            }

            if(ok)
            {
                DEBUG("Merging {} and {}", videoFile->path().string(), audioFile->path().string());
                Hollywood::MuxFilesSync(videoFile->path().string(), audioFile->path().string(), (tempDir / "video.mp4").string());
                std::filesystem::copy(tempDir / "video.mp4", path(video->levelDir.value()) / video->GetVideoFileName(video->levelDir.value()));
            }
            else 
            {
                video->downloadState = DownloadState::NotDownloaded;
                video->errorMessage = "Failed to download video or audio file(s)";
            }
        }

        std::filesystem::remove_all(tempDir);
        {
            std::lock_guard lock(currentDownloadsMutex);
            // remove current download from the downloads list
            std::erase_if(currentDownloads, [&](const std::pair<std::shared_ptr<VideoConfig>, std::stop_source>& downloadInfo)
                { return downloadInfo.first == video; });
        }

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