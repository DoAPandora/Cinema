#include "main.hpp"
#include "UI/VideoMenuManager.hpp"
#include "ModConfig.hpp"
#include "Downloader.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "pinkcore/shared/RequirementAPI.hpp"

#include "bsml-lite/shared/Creation/Image.hpp"

#include <filesystem>
#include <thread>

DEFINE_TYPE(Cinema, VideoMenuManager);

const std::string videosDir = "/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Videos/";
const std::string thumbnailsDir = "/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Thumbnails/";

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
            mainVertical->get_gameObject()->SetActive(false);
            noCinemaVertical->get_gameObject()->SetActive(true);
            return;
        }

        mainVertical->get_gameObject()->SetActive(true);
        noCinemaVertical->get_gameObject()->SetActive(false);
        videoTitleText->SetText(currentLevelData.title);

        authorText->SetText(currentLevelData.author);
        durationText->SetText(ToDuration(currentLevelData.duration));

        bool isCurrentVideoDownloaded = std::filesystem::exists(videosDir + currentLevelData.videoFile);
        downloadButton->GetComponentInChildren<HMUI::CurvedTextMeshPro*>()->SetText(isCurrentVideoDownloaded ? "Delete" : "Download");
        videoOffsetText->SetText(std::to_string(currentLevelData.offset) + " ms");
        downloadState = isCurrentVideoDownloaded ? DownloadState::Downloaded : DownloadState::NotDownloaded;
        SetDownloadState(downloadState);
        SetCurrentThumbnail();
    }

    bool VideoMenuManager::ShouldCreateScreen()
    {
        return getModConfig().enabled.GetValue() ? doesCurrentSongUseCinema && downloadState == DownloadState::Downloaded : false;
    }

    std::string VideoMenuManager::GetCurrentVideoPath()
    {
        return videosDir + currentLevelData.videoFile;
    }

    std::string VideoMenuManager::ToDuration(int length)
    {
        int minutes = length / 60;
        int seconds = length - minutes * 60;
        std::string dur = std::to_string(minutes) + ":" + std::to_string(seconds);
        return dur;
    }

    void VideoMenuManager::SetDownloadState(DownloadState state)
    {
        downloadState = state;
        auto text = downloadButton->GetComponentInChildren<HMUI::CurvedTextMeshPro*>();

        switch(state) {
            case DownloadState::Downloaded:
                downloadStateText->SetText("Downloaded");
                downloadStateText->set_color(UnityEngine::Color::get_green());
                downloadButton->set_interactable(true);
                text->SetText("Delete");
                break;
            case DownloadState::Downloading:
                downloadStateText->SetText("Downloading...");
                downloadStateText->set_color(UnityEngine::Color::get_yellow());
                downloadButton->set_interactable(false);
                text->SetText("Downloading");
                break;
            case DownloadState::NotDownloaded:
                downloadStateText->SetText("Not Downloaded");
                downloadStateText->set_color(UnityEngine::Color::get_red());
                downloadButton->set_interactable(true);
                text->SetText("Download");
                break;
        }
    }

    void VideoMenuManager::DownloadButtonClicked()
    {
        if(downloadState == DownloadState::Downloaded)
        {
            DeleteCurrentVideo();
            SetDownloadState(DownloadState::NotDownloaded);
            return;
        }

        getLogger().info("Downloading video, disabling play button...");
        PinkCore::RequirementAPI::DisablePlayButton(getModInfo());
        SetDownloadState(DownloadState::Downloading);
        std::thread([this]()
        {
            bool result = DownloadCurrentVideo();
            if(!result)
            {
                getLogger().info("Download failed");
                std::string path = videosDir + get_currentLevelData().videoId + ".mp4";
                if(std::filesystem::exists(path))
                    std::filesystem::remove(path);
            }
            else
                std::filesystem::rename(videosDir + currentLevelData.videoId + ".mp4", videosDir + currentLevelData.videoFile);

            QuestUI::MainThreadScheduler::Schedule([this]()
            {
                UpdateMenu();
                PinkCore::RequirementAPI::EnablePlayButton(getModInfo());
                progressBar->canvas->get_gameObject()->SetActive(false);
            });
        }).detach();

    }

    void VideoMenuManager::DeleteCurrentVideo()
    {
        if(downloadState == DownloadState::Downloaded)
            std::filesystem::remove(GetCurrentVideoPath());
    }

    void VideoMenuManager::UpdateProgressBar(float progress)
    {
        if(!progressBar)
            progressBar = QuestUI::BeatSaberUI::CreateProgressBar({0, 3, 4}, "Downloading Video...", "Cinema");
        progressBar->canvas->get_gameObject()->SetActive(true);
        progressBar->SetProgress(progress);
        progressBar->subText2->SetText(std::to_string(int(progress)) + "%");
    }

    bool VideoMenuManager::DownloadCurrentVideo()
    {
        std::string url = "https://youtu.be/" + currentLevelData.videoId;
        bool result = Downloader::DownloadVideo(url, [this](float progress)
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

    void VideoMenuManager::SetCurrentThumbnail()
    {
        getLogger().info("Attempting to set thumbnail");
        std::string path = thumbnailsDir + currentLevelData.videoId + ".webp";
        if(std::filesystem::exists(path))
        {
            getLogger().info("Thumbnail alreadye exists");
            thumbnailSprite->set_sprite(BSML::Lite::FileToSprite(path));
            return;
        }

        std::thread([this, path]()
        {
            getLogger().info("Downloading sprite");
            if(!DownloadCurrentThumbnail())
            {
                getLogger().info("Downloading thumbnail failed");
                return;
            }
            QuestUI::MainThreadScheduler::Schedule([this, path]()
            {
                getLogger().info("Downloaded Sprite");
                if(std::filesystem::exists(path))
                    this->thumbnailSprite->set_sprite(BSML::Lite::FileToSprite(path));
            });
        }).detach();
    }

    bool VideoMenuManager::DownloadCurrentThumbnail()
    {
        return Downloader::DownloadThumbnail("https://youtu.be/" + currentLevelData.videoId, [](float progress){
            getLogger().info("Thumnail download progress: %f", progress);
        });
    }

    void VideoMenuManager::DecreaseOffsetMajor()
    {
        currentLevelData.offset -= 1000;
        UpdateMenu();
    }

    void VideoMenuManager::DecreaseOffsetNormal()
    {
        currentLevelData.offset -= 100;
        UpdateMenu();
    }

    void VideoMenuManager::DecreaseOffsetMinor()
    {
        currentLevelData.offset -= 10;
        UpdateMenu();
    }

    void VideoMenuManager::IncreaseOffsetMinor()
    {
        currentLevelData.offset += 10;
        UpdateMenu();
    }

    void VideoMenuManager::IncreaseOffsetNormal()
    {
        currentLevelData.offset += 100;
        UpdateMenu();
    }

    void VideoMenuManager::IncreaseOffsetMajor()
    {
        currentLevelData.offset += 1000;
        UpdateMenu();
    }


    void VideoMenuManager::ctor() { }

    bool VideoMenuManager::get_doesCurrentSongUseCinema() { return this->doesCurrentSongUseCinema; }
    void VideoMenuManager::set_doesCurrentSongUseCinema(bool value) { this->doesCurrentSongUseCinema = value; }

    VideoMenuManager::DownloadState VideoMenuManager::get_downloadState() { return this->downloadState; }
    void VideoMenuManager::set_downloadState(VideoMenuManager::DownloadState value) { this->downloadState = value; }

    JSON::CinemaInfo VideoMenuManager::get_currentLevelData() { return this->currentLevelData; }
    void VideoMenuManager::set_currentLevelData(JSON::CinemaInfo value) { this->currentLevelData = value; }
}