#include "main.hpp"
#include "WIPUI/VideoMenuManager.hpp"
#include "ModConfig.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "pythonlib/shared/Python.hpp"
#include "pythonlib/shared/Utils/FileUtils.hpp"
#include "pythonlib/shared/Utils/StringUtils.hpp"
#include "assets.hpp"

#include "pinkcore/shared/RequirementAPI.hpp"

#include <filesystem>
#include <thread>

DEFINE_TYPE(Cinema, VideoMenuManager);

const std::string modDataDir = "/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/";

namespace Cinema
{
    SafePtr<VideoMenuManager> VideoMenuManager::instance;
    VideoMenuManager *VideoMenuManager::get_instance()
    {
        if (!instance)
            instance = VideoMenuManager::New_ctor();
        return instance.ptr();
    }

    void VideoMenuManager::UpdateMenu()
    {
        if(!doesCurrentSongUseCinema)
        {
            // Make text stuff
            return;
        }

        videoTitleText->SetText(currentLevelData.title);

        isCurrentSongDownloaded = std::filesystem::exists(modDataDir + currentLevelData.videoFile);
        downloadButton->set_interactable(!isCurrentSongDownloaded);
    }

    bool VideoMenuManager::ShouldCreateScreen()
    {
        return getModConfig().enabled.GetValue() ? doesCurrentSongUseCinema && isCurrentSongDownloaded : false;
    }

    std::string VideoMenuManager::GetCurrentVideoPath()
    {
        return modDataDir + currentLevelData.videoFile;
    }

    void VideoMenuManager::DownloadButtonClicked()
    {
        getLogger().info("Downloading video, disabling play button...");
        PinkCore::RequirementAPI::DisablePlayButton(getModInfo());
        std::thread([this]()
        {
            bool result = DownloadCurrentVideo();
            if(!result)
            {
                getLogger().info("Download failed");
                std::string path = modDataDir + get_currentLevelData().videoId + ".mp4";
                if(std::filesystem::exists(path))
                    std::filesystem::remove(path);
            }
            else
                std::filesystem::rename(modDataDir + currentLevelData.videoId + ".mp4", modDataDir + currentLevelData.videoFile);

            QuestUI::MainThreadScheduler::Schedule([this]()
            {
                UpdateMenu();
                PinkCore::RequirementAPI::EnablePlayButton(getModInfo());
                progressBar->canvas->get_gameObject()->SetActive(false);
            });
        }).detach();

    }

    void VideoMenuManager::UpdateProgressBar(float progress)
    {
        if(!progressBar)
            progressBar = QuestUI::BeatSaberUI::CreateProgressBar({3, 4, 0}, "Downloading Video...", "Cinema");
        progressBar->canvas->get_gameObject()->SetActive(true);
        progressBar->SetProgress(progress);
        progressBar->subText2->SetText(std::to_string(int(progress * 100)) + "%");
    }

    bool VideoMenuManager::DownloadCurrentVideo()
    {
        std::string url = "https://youtu.be/" + currentLevelData.videoId;
        bool result = DownloadVideo(url, [this](float progress)
        {
            getLogger().info("Video download progress: %f", progress);
            QuestUI::MainThreadScheduler::Schedule([progress, this]()
            {
                UpdateProgressBar(progress);
            });
        });
        getLogger().info("Finished downloading video!");
        return result;
    }

    bool VideoMenuManager::DownloadVideo(std::string_view url, std::function<void(float)> status)
    {
        bool error = false;
        std::function<void(int, char*)> eventHandler = [status, &error](int type, char* data) {
            switch (type) {
            case 0:
                {
                    std::string dataString(data);
                    if(dataString.find("[download]", 0) != -1) {
                        auto pos = dataString.find("%", 0);
                        if(pos != -1 && pos > 5) {
                            auto percentange = dataString.substr(pos-5, 5);
                            if(percentange.find("]", 0) == 0) 
                                percentange = percentange.substr(1);
                            status(std::stof(percentange));
                        }
                    }
                }
                break;
            case 1:
                error = true;
                getLogger().info("Error: %s", data);
                break;
            }
        };
        Python::PythonWriteEvent += eventHandler;
        std::string ytdlp = FileUtils::getScriptsPath() + "/yt_dlp";
        if(!direxists(ytdlp))
            FileUtils::ExtractZip(IncludedAssets::ytdlp_zip, ytdlp);
        Python::PyRun_SimpleString("from yt_dlp.__init__ import _real_main");
        std::string command = "_real_main([";
        for(auto splitted : StringUtils::Split("--no-cache-dir -o %(id)s.%(ext)s -P /sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema " + url, " ")) {
            command += "\"" + splitted + "\",";
        }
        command = command.substr(0, command.length()-1) + "])";
        int result = Python::PyRun_SimpleString(command.c_str());
        Python::PythonWriteEvent -= eventHandler;
        return !error;
    }


    void VideoMenuManager::ctor() { }

    bool VideoMenuManager::get_doesCurrentSongUseCinema() { return this->doesCurrentSongUseCinema; }
    void VideoMenuManager::set_doesCurrentSongUseCinema(bool value) { this->doesCurrentSongUseCinema = value; }

    bool VideoMenuManager::get_isCurrentSongDownloaded() { return this->isCurrentSongDownloaded; }
    void VideoMenuManager::set_isCurrentSongDownloaded(bool value) { this->isCurrentSongDownloaded = value; }

    JSON::CinemaInfo VideoMenuManager::get_currentLevelData() { return this->currentLevelData; }
    void VideoMenuManager::set_currentLevelData(JSON::CinemaInfo value) { this->currentLevelData = value; }
}