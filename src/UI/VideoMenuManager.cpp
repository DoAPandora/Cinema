#include "main.hpp"
#include "UI/VideoMenuManager.hpp"

#include "Downloader.hpp"
#include "Screen/CustomVideoPlayer.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "GlobalNamespace/HMTask.hpp"

#include "pinkcore/shared/RequirementAPI.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include <filesystem>
#include <thread>

DEFINE_TYPE(Cinema, VideoMenuManager);

// this file is so messy

#define DEFINE_OFFSET_CHANGE(name, change) \
void VideoMenuManager::name() { currentVideoConfig.offset += change; UpdateMenu(); }

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
        videoTitleText->SetText(currentVideoConfig.title.value());

        authorText->SetText(currentVideoConfig.author.value());
        durationText->SetText(ToDuration(currentVideoConfig.duration));

        bool isCurrentVideoDownloaded = std::filesystem::exists(VIDEO_DIR + currentVideoConfig.videoFile.value());
        downloadButton->GetComponentInChildren<HMUI::CurvedTextMeshPro*>()->SetText(isCurrentVideoDownloaded ? "Delete" : "Download");
        videoOffsetText->SetText(std::to_string(currentVideoConfig.offset) + " ms");
        downloadState = isCurrentVideoDownloaded ? DownloadState::Downloaded : DownloadState::NotDownloaded;
        SetDownloadState(downloadState);
        SetCurrentThumbnail();
    }

    bool VideoMenuManager::ShouldCreateScreen()
    {
        return getModConfig().enabled.GetValue() ? doesCurrentSongUseCinema && downloadState == DownloadState::Downloaded : false;
    }

    std::string VideoMenuManager::ToDuration(int length)
    {
        int minutes = length / 60;
        int seconds = length - minutes * 60;
        std::string dur = std::to_string(minutes) + ":";
        if(seconds < 10)
            dur += "0";
        dur += std::to_string(seconds);
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

    void VideoMenuManager::DownloadButtonPressed()
    {
        if(downloadState == DownloadState::Downloaded)
        {
            DeleteCurrentVideo();
            SetDownloadState(DownloadState::NotDownloaded);
            return;
        }

        INFO("Downloading video, disabling play button...");
        PinkCore::RequirementAPI::DisablePlayButton(modInfo);
        SetDownloadState(DownloadState::Downloading);
        std::thread([this]()
        {
            bool result = DownloadCurrentVideo();
            if(!result)
            {
                INFO("Download failed");
                std::string path = VIDEO_DIR + currentVideoConfig.videoID.value() + ".mp4";
                if(std::filesystem::exists(path))
                    std::filesystem::remove(path);
            }
            else
            {
                std::filesystem::rename(VIDEO_DIR + currentVideoConfig.videoID.value() + ".mp4", VIDEO_DIR + currentVideoConfig.videoFile.value());
            }

            QuestUI::MainThreadScheduler::Schedule([this]()
            {
                UpdateMenu();
                PinkCore::RequirementAPI::EnablePlayButton(modInfo);
                progressBar->canvas->get_gameObject()->SetActive(false);
            });
        }).detach();

    }

    void VideoMenuManager::PreviewButtonPressed()
    {
    }

    void VideoMenuManager::DeleteCurrentVideo()
    {
        if(downloadState == DownloadState::Downloaded)
            std::filesystem::remove(currentVideoConfig.get_videoPath());
    }

    void VideoMenuManager::UpdateProgressBar(float progress)
    {
        if(!progressBar)
            progressBar = QuestUI::BeatSaberUI::CreateProgressBar(UnityEngine::Vector3(0, 3.3, 4), "Downloading Video...", "Cinema");
        progressBar->canvas->get_gameObject()->SetActive(true);
        progressBar->SetProgress(progress / 100);
        progressBar->subText2->SetText(std::to_string(int(progress)) + "%");
    }

    bool VideoMenuManager::DownloadCurrentVideo()
    {
        bool result = Downloader::DownloadVideo(currentVideoConfig.videoID.value(), [this](float progress)
        {
            INFO("Video download progress: %f", progress);
            QuestUI::MainThreadScheduler::Schedule([progress, this]()
            {
                UpdateProgressBar(progress);
            });
        });
        INFO("Finished downloading video!");
        return result;
    }

    void VideoMenuManager::SetCurrentThumbnail()
    {
        std::string path = THUMBNAIL_DIR + currentVideoConfig.videoID.value() + ".jpg";
        INFO("%s", path.c_str());
        if(std::filesystem::exists(path))
        {
            thumbnailSprite->set_sprite(QuestUI::BeatSaberUI::FileToSprite(path));
        }
        else
            DownloadCurrentThumbnail();
    }

    void VideoMenuManager::DownloadCurrentThumbnail()
    {
        Downloader::DownloadThumbnail(currentVideoConfig.videoID.value(), [this]()
        {
            INFO("Finished downloading thumbnail");
            SetCurrentThumbnail();
        });    
    }

    DEFINE_OFFSET_CHANGE(DecreaseOffsetMajor, -1000);
    DEFINE_OFFSET_CHANGE(DecreaseOffsetNormal, -100);
    DEFINE_OFFSET_CHANGE(DecreaseOffsetMinor, -10)
    DEFINE_OFFSET_CHANGE(IncreaseOffsetMinor, 10);
    DEFINE_OFFSET_CHANGE(IncreaseOffsetNormal, 100);
    DEFINE_OFFSET_CHANGE(IncreaseOffsetMajor, 1000);

    void VideoMenuManager::ctor() { }
}