#include "VideoMenu/VideoMenu.hpp"
#include "Screen/PlaybackController.hpp"
#include "Util/Events.hpp"
#include "Video/VideoConfig.hpp"
#include "Video/VideoLoader.hpp"
#include "assets.hpp"
#include "main.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/utilities.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"

DEFINE_TYPE(Cinema, VideoMenu);
DEFINE_TYPE(Cinema, VideoMenuStatus);

namespace Cinema
{

    UnityW<VideoMenu> VideoMenu::get_instance()
    {
        if(!instance)
            instance = UnityEngine::GameObject::New_ctor("CinemaVideoMenu")->AddComponent<VideoMenu*>();
        return instance;
    }

    bool VideoMenu::get_CustomizeOffset()
    {
        return currentVideo != nullptr && (currentVideo->IsOfficialConfig || (currentVideo->userSettings.has_value() && currentVideo->userSettings.value().customOffset) || currentVideo->IsWIPLevel);
    }

    void VideoMenu::set_CustomizeOffset(bool value)
    {
        if(currentVideo == nullptr || !value)
        {
            return;
        }

        currentVideo->userSettings = currentVideo->userSettings.value_or(VideoConfig::UserSettings());
        currentVideo->userSettings->customOffset = true;
        currentVideo->userSettings->originalOffset = currentVideo->offset;
        currentVideo->needsToSave = true;
        customizeOffsetToggle->SetActive(false);
        offsetControls->SetActive(true);
    }

    void VideoMenu::Init()
    {
        Events::levelSelected -= {&VideoMenu::OnLevelSelected, this};
        Events::levelSelected += {&VideoMenu::OnLevelSelected, this};
        Events::difficultySelected -= {&VideoMenu::OnDifficultySelected, this};
        Events::difficultySelected += {&VideoMenu::OnDifficultySelected, this};

        if(!rootObject)
        {
            return;
        }

        previewButtonText = previewButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
        deleteVideoButtonText = deleteVideoButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>();

        CreateStatusListener();
        auto scale = UnityEngine::Vector3::op_Multiply(deleteConfigButton->transform->localScale, 0.5f);
        deleteConfigButton->get_transform()->set_localScale(scale);

        searchKeyboard->clearOnOpen = false;

        if(videoMenuInitialized)
        {
            return;
        }

        videoMenuInitialized = true;
        videoDetails->get_gameObject()->SetActive(false);
        videoSearchResults->get_gameObject()->SetActive(false);

        downloadController.onDownloadProgress += [this](std::shared_ptr<VideoConfig> video) {
            BSML::MainThreadScheduler::Schedule(std::bind(&VideoMenu::OnDownloadProgress, this, std::forward<decltype(video)>(video)));
        };
        downloadController.onDownloadFinished += [this](std::shared_ptr<VideoConfig> video) {
            BSML::MainThreadScheduler::Schedule(std::bind(&VideoMenu::OnDownloadFinished, this, std::forward<decltype(video)>(video)));
        };
    }

    void VideoMenu::CreateStatusListener()
    {
        if(menuStatus)
        {
            menuStatus->didEnable -= {&VideoMenu::StatusViewerDidEnable, this};
            menuStatus->didDisable -= {&VideoMenu::StatusViewerDidDisable, this};
            UnityEngine::Object::Destroy(menuStatus);
        }

        menuStatus = rootObject->AddComponent<VideoMenuStatus*>();
        DEBUG("Adding status listener");
        menuStatus->didEnable += {&VideoMenu::StatusViewerDidEnable, this};
        menuStatus->didDisable += {&VideoMenu::StatusViewerDidDisable, this};
    }

    void VideoMenu::AddTab()
    {
        DEBUG("Adding tab");
        BSML::Register::RegisterGameplaySetupTab("Cinema", [this](UnityEngine::GameObject* go, bool firstActivation)
                                                 {
            if (firstActivation)
            {
                auto parser = BSML::parse_and_construct(IncludedAssets::VideoMenu_bsml, go->get_transform(), this);
                bsmlParserParams = parser->parserParams;
                Init();
                StatusViewerDidEnable();
            } });
    }

    void VideoMenu::RemoveTab()
    {
        DEBUG("Removing tab");
        BSML::Register::UnRegisterGameplaySetupTab("Cinema");
    }

    void VideoMenu::ResetVideoMenu()
    {
        DEBUG("BSMLParserParams: {}", fmt::ptr(bsmlParserParams.get()));
        // bsmlParserParams->EmitEvent("hide-keyboard");
        noVideoBg->get_gameObject()->SetActive(true);
        videoDetails->get_gameObject()->SetActive(false);
        SetButtonState(false);

        if(!getModConfig().enabled.GetValue())
        {
            noVideoText->SetTextInternal("Cinema is disabled.\\r\\nYou can re-enable it on the left side of the main menu.");
            return;
        }

        if(!currentLevel)
        {
            noVideoText->SetTextInternal("No level Selected");
            return;
        }

        noVideoText->SetTextInternal("No video configured");
    }

    void VideoMenu::OnDownloadProgress(std::shared_ptr<VideoConfig> videoConfig)
    {
        BSML::MainThreadScheduler::Schedule([this, videoConfig]()
        {
            UpdateStatusText(videoConfig);
            SetupLevelDetailView(videoConfig);
        });
    }

    void VideoMenu::SetButtonState(bool state)
    {
        previewButton->set_interactable(state);
        deleteConfigButton->set_interactable(state);
        deleteVideoButton->set_interactable(state);
        searchButton->get_gameObject()->SetActive(currentLevel != nullptr);
        //        previewButtonText->SetTextInternal(PlaybackController::get_instance)

        if(currentLevel)
        {
            CheckEntitlementAndEnableSearch(currentLevel);
        }

        if(currentVideo == nullptr)
        {
            return;
        }

        auto officialConfig = currentVideo->configByMapper.value();
        deleteConfigButton->get_gameObject()->SetActive(!officialConfig);

        switch(currentVideo->downloadState)
        {
        case DownloadState::Converting:
        case DownloadState::Preparing:
        case DownloadState::Downloading:
        case DownloadState::DownloadingVideo:
        case DownloadState::DownloadingAudio:
            {
                deleteVideoButtonText->SetTextInternal("Cancel");
                previewButton->set_interactable(false);
                deleteVideoButton->get_transform()->Find(
                                                      "Underline")
                    ->get_gameObject()
                    ->GetComponent<UnityEngine::UI::Image*>()
                    ->set_color(
                        UnityEngine::Color::get_gray());
                break;
            }
        case DownloadState::NotDownloaded:
        case DownloadState::Cancelled:
            {
                deleteVideoButtonText->text = "Download";
                deleteVideoButton->set_interactable(false);
                auto underlineColor = UnityEngine::Color::get_clear();
                if(state)
                {
                    underlineColor = UnityEngine::Color::get_green();
                    deleteVideoButton->set_interactable(true);
                }

                deleteVideoButton->get_transform()->Find(
                                                      "Underline")
                    ->get_gameObject()
                    ->GetComponent<UnityEngine::UI::Image*>()
                    ->set_color(
                        underlineColor);
                previewButton->set_interactable(false);
                break;
            }
        default:
            {
                deleteVideoButtonText->SetTextInternal("Delete Video");
                deleteVideoButton->get_transform()->Find(
                                                      "Underline")
                    ->get_gameObject()
                    ->GetComponent<UnityEngine::UI::Image*>()
                    ->set_color(
                        UnityEngine::Color::get_gray());
                previewButton->set_interactable(state);
                break;
            }
        }
    }

    void VideoMenu::CheckEntitlementAndEnableSearch(GlobalNamespace::BeatmapLevel* level)
    {
        // auto entitlement =
    }

    void VideoMenu::SetupVideoDetails()
    {
        if(!videoSearchResults)
        {
            WARN("Video search results view rect is null, skipping UI setup");
            return;
        }

        videoSearchResults->get_gameObject()->SetActive(false);

        if(currentVideo == nullptr)
        {
            DEBUG("Current video is null");
            ResetVideoMenu();
            return;
        }

        SetupLevelDetailView(currentVideo);

        if(!videoMenuActive)
        {
            DEBUG("VideoMenu not active");
            ResetVideoMenu();
            return;
        }

        if(!currentVideo->videoID.has_value() && !currentVideo->videoUrl.has_value())
        {
            ResetVideoMenu();
            if(!currentVideo->forceEnvironmentModifications)
            {
                return;
            }

            noVideoText->SetTextInternal("This map uses Cinema to modify the environment\\r\\nwithout displaying a video.\\r\\n\\r\\nNo configuration options available.");
            searchButton->set_interactable(false);
            searchButton->get_gameObject()->SetActive(false);

            return;
        }

        noVideoBg->get_gameObject()->SetActive(false);
        videoDetails->get_gameObject()->SetActive(true);

        SetButtonState(true);

        videoTitle->SetTextInternal(currentVideo->title.value_or("Untitled Video"));
        videoAuthor->SetTextInternal("Author: " + currentVideo->author.value_or("Unknown Author"));
        videoDuration->SetTextInternal("Duration: " + std::to_string(currentVideo->duration));

        videoOffset->SetTextInternal(fmt::format("{:10L} ms", currentVideo->offset));
        SetThumbnail(currentVideo->videoID.has_value() ? std::make_optional(fmt::format("https://i.ytimg.com/vi/{}/hqdefault.jpg", currentVideo->videoID.value())) : std::nullopt);

        UpdateStatusText(currentVideo);
        if(CustomizeOffset)
        {
            customizeOffsetToggle->SetActive(false);
            offsetControls->SetActive(true);
        }
        else
        {
            customizeOffsetToggle->SetActive(true);
            offsetControls->SetActive(false);
        }

        UpdateStatusText(currentVideo);

        // bsmlParserParams->EmitEvent("update-customize-offset");
    }

    void VideoMenu::SetupLevelDetailView(std::shared_ptr<VideoConfig> videoConfig)
    {
        if(videoConfig != currentVideo)
        {
            return;
        }

        if((!currentVideo->videoID.has_value() && !currentVideo->videoUrl.has_value()))
        {
            return;
        }

        switch(videoConfig->downloadState)
        {
        case DownloadState::Downloaded:
            if(videoConfig->IsWIPLevel && !difficultyData->HasCinema() && !extraSongData->HasCinemaInAnyDifficulty())
            {
            }
            else if(!videoConfig->errorMessage.empty())
            {}
            else
            {}
        }
    }

    void VideoMenu::UpdateStatusText(std::shared_ptr<VideoConfig> videoConfig) 
    {
        if(videoConfig != currentVideo || !videoMenuActive)
        {
            return;
        }

        using UnityEngine::Color;
        switch (videoConfig->downloadState)
        {
            case DownloadState::Downloaded:
                videoStatus->text = "Downloaded";
                videoStatus->color = Color::get_green();
                previewButton->interactable = true;
                break;
            case DownloadState::Preparing:
                videoStatus->text = "Preparing...";
                videoStatus->color = Color::get_yellow();
                previewButton->interactable = false;
                break;
            case DownloadState::Downloading:
                videoStatus->text = fmt::format("Downloading {:.1f}%", videoConfig->downloadProgress);
                videoStatus->color = Color::get_yellow();
                previewButton->interactable = false;
                break;
            case DownloadState::NotDownloaded:
                videoStatus->text = "Not downloaded";
                videoStatus->color = Color::get_red();
                previewButton->interactable = false;
                break;
            case DownloadState::Cancelled:
                videoStatus->text = "Cancelled";
                videoStatus->color = Color::get_red();
                previewButton->interactable = false;
                break;

        }
    }

    void VideoMenu::SetThumbnail(std::optional<std::string> url)
    {
        if(url.has_value() && url == thumbnailURL)
        {
            return;
        }

        thumbnailURL = url;

        if(!url.has_value())
        {
            SetThumbnailFromCover(currentLevel);
            return;
        }

        // BSML::Utilities::SetImage(videoThumbnail, url.value());
    }

    void VideoMenu::SetThumbnailFromCover(GlobalNamespace::BeatmapLevel* level) {}

    void VideoMenu::SetSelectedLevel(GlobalNamespace::BeatmapLevel* level)
    {
        if(currentLevel && currentLevel->levelID == level->levelID)
        {
            return;
        }

        DEBUG("Setting level to {}", level->levelID);
        HandleDidSelectLevel(level);
    }

    void VideoMenu::HandleDidSelectLevel(GlobalNamespace::BeatmapLevel* level, bool isPlaylistSong)
    {
        extraSongData = std::nullopt;
        difficultyData = std::nullopt;

        if(!getModConfig().enabled.GetValue() || !level)
        {
            return;
        }

        PlaybackController::get_instance()->StopPreview(true);

        if(currentVideo != nullptr && currentVideo->needsToSave)
        {
            VideoLoader::SaveVideoConfig(currentVideo);
        }

        currentLevelIsPlaylistSong = isPlaylistSong;
        currentLevel = level;
        if(!currentLevel)
        {
            currentVideo = nullptr;
            PlaybackController::get_instance()->SetSelectedLevel(nullptr, nullptr);
            SetupVideoDetails();
            return;
        }

        currentVideo = VideoLoader::GetConfigForLevel(level);
        PlaybackController::get_instance()->SetSelectedLevel(currentLevel, currentVideo);
        SetupVideoDetails();
    }

    void VideoMenu::OnLevelSelected(GlobalNamespace::BeatmapLevel* level)
    {
        HandleDidSelectLevel(level);
    }

    void VideoMenu::OnDifficultySelected(Cinema::ExtraSongDataArgs extraSongDataArgs)
    {
        extraSongData = extraSongDataArgs.songData;
        difficultyData = extraSongDataArgs.selectedDifficultyData;
        if(currentVideo != nullptr)
        {
            SetupLevelDetailView(currentVideo);
        }
    }

    void VideoMenu::OnConfigChanged(std::shared_ptr<VideoConfig> config) {}

    void VideoMenu::StatusViewerDidEnable()
    {
        DEBUG("VideoMenu Active");
        videoMenuActive = true;
    }

    void VideoMenu::StatusViewerDidDisable()
    {
        DEBUG("VideoMenu Inactive");
        videoMenuActive = false;
    }

    void VideoMenu::ApplyOffset(int offset) {}

    void VideoMenu::SearchAction() {}

    void VideoMenu::OnDownloadFinished(std::shared_ptr<VideoConfig> video)
    {
        BSML::MainThreadScheduler::Schedule([this, video]()
        {
            UpdateStatusText(video);
        });
    }

    void VideoMenu::ShowKeyboard()
    {
        // bsmlParserParams->EmitEvent("show-keyboard");
    }

    void VideoMenu::OnRefineAction() {}

    void VideoMenu::OnDeleteVideoAction()
    {
        if(!currentVideo)
        {
            WARN("Current video was null on delete action");
            return;
        }

        PlaybackController::get_instance()->StopPreview(true);

        switch(currentVideo->downloadState)
        {
        case DownloadState::Preparing:
        case DownloadState::Downloading:
        case DownloadState::DownloadingAudio:
        case DownloadState::DownloadingVideo:
            {
                downloadController.CancelDownload(currentVideo);
                break;
            }
        case DownloadState::NotDownloaded:
        case DownloadState::Cancelled:
            {
                currentVideo->downloadProgress = 0;
                downloadController.StartDownload(currentVideo);
                currentVideo->needsToSave = true;
                VideoLoader::AddConfigToCache(currentVideo, currentLevel);
                break;
            }
        default:
            {
                VideoLoader::DeleteVideo(currentVideo);
                PlaybackController::get_instance()->videoPlayer->FadeOut(0.2f);
                SetupLevelDetailView(currentVideo);
            }
        }

        UpdateStatusText(currentVideo);
    }

    void VideoMenu::OnDeleteConfigAction() {}

    void VideoMenu::OnBackAction() {}

    void VideoMenu::OnQueryAction(StringW query) {}

    void VideoMenu::SearchFinished() {}

    void VideoMenu::ResetSearchView() {}

    custom_types::Helpers::Coroutine VideoMenu::SearchLoadingCoroutine() {}

    void VideoMenu::OnSelectCell(HMUI::TableView* view, int idx) {}

    void VideoMenu::OnDownloadAction() {}

    void VideoMenu::OnPreviewAction() {}

    void VideoMenu::DecreaseOffsetHigh() {}

    void VideoMenu::DecreaseOffsetMid() {}

    void VideoMenu::DecreaseOffsetLow() {}

    void VideoMenu::IncreaseOffsetHigh() {}

    void VideoMenu::IncreaseOffsetMid() {}

    void VideoMenu::IncreaseOffsetLow() {}

    void VideoMenuStatus::OnEnable()
    {
        didEnable.invoke();
    }

    void VideoMenuStatus::OnDisable()
    {
        didDisable.invoke();
    }
} // namespace Cinema