#include "main.hpp"
#include "VideoMenu/VideoMenu.hpp"
#include "Util/Events.hpp"
#include "assets.hpp"
#include "Video/VideoLoader.hpp"
#include "Screen/PlaybackController.hpp"

#include "bsml/shared/BSML.hpp"

DEFINE_TYPE(Cinema, VideoMenu);
DEFINE_TYPE(Cinema, VideoMenuStatus);

namespace Cinema {

    VideoMenu* VideoMenu::get_instance()
    {
        if (!instance || !instance->m_CachedPtr.m_value)
            instance = UnityEngine::GameObject::New_ctor("CinemaVideoMenu")->AddComponent<VideoMenu*>();
        return instance;
    }

    bool VideoMenu::get_CustomizeOffset()
    {
        return currentVideo != std::nullopt && (currentVideo->IsOfficialConfig || (currentVideo->userSettings.has_value() && currentVideo->userSettings.value().customOffset) || currentVideo->IsWIPLevel);
    }

    void VideoMenu::set_CustomizeOffset(bool value)
    {
        if (currentVideo == std::nullopt || !value)
        {
            return;
        }

        currentVideo->userSettings = currentVideo->userSettings.value_or(UserSettings());
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

        if (!rootObject || !rootObject->m_CachedPtr.m_value)
        {
            return;
        }

        previewButtonText = previewButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
        deleteVideoButtonText = deleteVideoButton->GetComponentInChildren<TMPro::TextMeshProUGUI*>();

        CreateStatusListener();
        deleteConfigButton->get_transform()->set_localScale(deleteConfigButton->get_transform()->get_localScale() * 0.5f);

        searchKeyboard->clearOnOpen = false;

        if (videoMenuInitialized)
        {
            return;
        }

        videoMenuInitialized = true;
        DEBUG("DisablingUI");
        videoDetails->get_gameObject()->SetActive(false);
        videoSearchResults->get_gameObject()->SetActive(false);

    }

    void VideoMenu::CreateStatusListener()
    {
        if (menuStatus && menuStatus->m_CachedPtr.m_value)
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
                bsmlParserParams = parser->parserParams.get();
                Init();
            }
        });
    }

    void VideoMenu::RemoveTab()
    {
        DEBUG("Removing tab");
        BSML::Register::UnRegisterGameplaySetupTab("Cinema");
    }

    void VideoMenu::ResetVideoMenu()
    {
        bsmlParserParams->EmitEvent("hide-keyboard");
        noVideoBg->get_gameObject()->SetActive(true);
        videoDetails->get_gameObject()->SetActive(false);
        SetButtonState(false);

        if (!getModConfig().enabled.GetValue())
        {
             noVideoText->SetText("Cinema is disabled.\\r\\nYou can re-enable it on the left side of the main menu.");
            return;
        }

        if (!currentLevel)
        {
            noVideoText->SetText("No level Selected");
            return;
        }

        noVideoText->SetText("No video configured");
    }

    void VideoMenu::OnDownloadProgress(const Cinema::VideoConfig &videoConfig)
    {
        UpdateStatustext(videoConfig);
        SetupLevelDetailView(videoConfig);
    }

    void VideoMenu::SetButtonState(bool state)
    {
        previewButton->set_interactable(state);
        deleteConfigButton->set_interactable(state);
        deleteVideoButton->set_interactable(state);
        searchButton->get_gameObject()->SetActive(currentLevel != nullptr);
//        previewButtonText->SetText(PlaybackController::get_instance)

        if (currentLevel)
        {
            CheckEntitlementAndEnableSearch(currentLevel);
        }

        if (currentVideo == std::nullopt)
        {
            return;
        }

        auto officialConfig = *currentVideo->configByMapper;
        deleteConfigButton->get_gameObject()->SetActive(!officialConfig);

        switch (currentVideo->downloadState) {
            case DownloadState::Converting:
            case DownloadState::Preparing:
            case DownloadState::Downloading:
            case DownloadState::DownloadingVideo:
            case DownloadState::DownloadingAudio: {
                deleteVideoButtonText->SetText("Cancel");
                previewButton->set_interactable(false);
                deleteVideoButton->get_transform()->Find(
                        "Underline")->get_gameObject()->GetComponent<UnityEngine::UI::Image *>()->set_color(
                        UnityEngine::Color::get_gray());
                break;
            }
            case DownloadState::NotDownloaded:
            case DownloadState::Cancelled: {
                deleteVideoButtonText->SetText("Download");
                deleteVideoButton->set_interactable(false);
                auto underlineColor = UnityEngine::Color::get_clear();
                if (state) {
                    underlineColor = UnityEngine::Color::get_green();
                    deleteVideoButton->set_interactable(true);
                }

                deleteVideoButton->get_transform()->Find(
                        "Underline")->get_gameObject()->GetComponent<UnityEngine::UI::Image *>()->set_color(
                        underlineColor);
                previewButton->set_interactable(false);
                break;
            }
            default: {
                deleteVideoButtonText->SetText("Delete Video");
                deleteVideoButton->get_transform()->Find(
                        "Underline")->get_gameObject()->GetComponent<UnityEngine::UI::Image *>()->set_color(
                        UnityEngine::Color::get_gray());
                previewButton->set_interactable(state);
                break;
            }
        }
    }

    void VideoMenu::CheckEntitlementAndEnableSearch(GlobalNamespace::IPreviewBeatmapLevel *level)
    {
        //auto entitlement =
    }

    void VideoMenu::SetupVideoDetails()
    {
        if (!videoSearchResults || videoSearchResults->m_CachedPtr.m_value)
        {
            WARN("Video search results view rect is null, skipping UI setup");
            return;
        }

        videoSearchResults->get_gameObject()->SetActive(false);
        if (currentVideo == std::nullopt)
        {
            ResetVideoMenu();
            return;
        }

        SetupLevelDetailView(*currentVideo);

        if(!videoMenuActive)
        {
            ResetVideoMenu();
            return;
        }

        if (currentVideo->videoID == std::nullopt && currentVideo->videoUrl == std::nullopt)
        {
            ResetVideoMenu();
            if (!currentVideo->forceEnvironmentModifications)
            {
                return;
            }

            noVideoText->SetText("This map uses Cinema to modify the environment\\r\\nwithout displaying a video.\\r\\n\\r\\nNo configuration options available.");
            searchButton->set_interactable(false);
            searchButton->get_gameObject()->SetActive(false);

            return;
        }

        noVideoBg->get_gameObject()->SetActive(false);
        videoDetails->get_gameObject()->SetActive(true);

        SetButtonState(true);

        videoTitle->SetText(currentVideo->title.value_or("Untitled Video"));
        videoAuthor->SetText("Author: " + currentVideo->author.value_or("Unknown Author"));
        videoDuration->SetText("Duration: " + std::to_string(currentVideo->duration));

        videoOffset->SetText(fmt::format("{:10L} ms", currentVideo->offset));
        SetThumbnail(currentVideo->videoID.has_value() ? fmt::format("https://i.ytimg.com/vi/{}/hqdefault.jpg", *currentVideo->videoID) : "");

        UpdateStatustext(*currentVideo);
        if (CustomizeOffset)
        {
            customizeOffsetToggle->SetActive(false);
            offsetControls->SetActive(true);
        }
        else
        {
            customizeOffsetToggle->SetActive(true);
            offsetControls->SetActive(false);
        }

        bsmlParserParams->EmitEvent("update-customize-offset");
    }

    void VideoMenu::SetupLevelDetailView(const Cinema::VideoConfig &videoConfig)
    {
        if (videoConfig != currentVideo)
        {
            return;
        }

        if ((currentVideo->videoID == std::nullopt && currentVideo->videoUrl == std::nullopt))
        {
            return;
        }
    }

    void VideoMenu::UpdateStatustext(const Cinema::VideoConfig &videoConfig) {}

    void VideoMenu::SetThumbnail(std::optional<std::string> url) {}

    void VideoMenu::SetThumbnailFromCover(GlobalNamespace::IPreviewBeatmapLevel *level) {}

    void VideoMenu::SetSelectedLevel(GlobalNamespace::IPreviewBeatmapLevel *level) {}

    void VideoMenu::HandleDidSelectLevel(GlobalNamespace::IPreviewBeatmapLevel *level, bool isPlaylistSong) {}

    void VideoMenu::OnLevelSelected(GlobalNamespace::IPreviewBeatmapLevel *level) {}

    void VideoMenu::OnDifficultySelected(Cinema::ExtraSongDataArgs extraSongDataArgs) {}

    void VideoMenu::OnConfigChanged(const std::optional<VideoConfig> &config) {}

    void VideoMenu::StatusViewerDidEnable() {}

    void VideoMenu::StatusViewerDidDisable() {}

    void VideoMenu::ApplyOffset(int offset) {}

    void VideoMenu::SearchAction() {}

    void VideoMenu::OnDownloadFinished(const Cinema::VideoConfig &video) {}

    void VideoMenu::ShowKeyboard()
    {
        bsmlParserParams->EmitEvent("show-keyboard");
    }

    void VideoMenu::OnRefineAction() {}

    void VideoMenu::OnDeleteVideoAction() {}

    void VideoMenu::OnDeleteConfigAction() {}

    void VideoMenu::OnBackAction() {}

    void VideoMenu::OnQueryAction(StringW query) {}

    void VideoMenu::SearchFinished() {}

    void VideoMenu::ResetSearchView() {}

    custom_types::Helpers::Coroutine VideoMenu::SearchLoadingCoroutine() {}

    void VideoMenu::OnSelectCell(HMUI::TableView *view, int idx) {}

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
}