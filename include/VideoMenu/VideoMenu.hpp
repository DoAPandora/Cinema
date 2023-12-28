#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "Util/ExtraSongData.hpp"
#include "Util/EventArgs.hpp"
#include "Video/VideoConfig.hpp"

#include "GlobalNamespace/PersistentSingleton_1.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/Coroutine.hpp"
#include "UnityEngine/MonoBehaviour.hpp"

#include "TMPro/TextMeshProUGUI.hpp"

#include "bsml/shared/BSML/Components/CustomListTableData.hpp"
#include "bsml/shared/BSML/Components/Keyboard/ModalKeyboard.hpp"
#include "bsml/shared/BSML/Parsing/BSMLParserParams.hpp"

DECLARE_CLASS_CODEGEN(Cinema, VideoMenuStatus, UnityEngine::MonoBehaviour,

public:

    UnorderedEventCallback<> didEnable;
    UnorderedEventCallback<> didDisable;

    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, OnDisable);

    DECLARE_DEFAULT_CTOR();
)

DECLARE_CLASS_CODEGEN(Cinema, VideoMenu, UnityEngine::MonoBehaviour,

    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, rootObject);
    DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, noVideoBg);
    DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, videoDetails);
    DECLARE_INSTANCE_FIELD(UnityEngine::RectTransform*, videoSearchResults);
    DECLARE_INSTANCE_FIELD(BSML::CustomListTableData*, videoList);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, searchResultsLoading);
    DECLARE_INSTANCE_FIELD(BSML::ModalKeyboard*, searchKeyboard);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, noVideoText);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, videoTitle);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, videoAuthor);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, videoDuration);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, videoStatus);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, videoOffset);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Image*, videoThumbnail);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, previewButtonText);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, previewButton);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, searchButton);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, deleteConfigButton);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, deleteVideoButton);
    DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, deleteVideoButtonText);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, downloadButton);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, offsetControls);
    DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, customizeOffsetToggle);

    DECLARE_INSTANCE_FIELD(UnityEngine::Coroutine*, searchLoadingCoroutine);
    DECLARE_INSTANCE_FIELD(UnityEngine::Coroutine*, updateSearchResultsCoroutine);

    //Menu status + detail view controller
    DECLARE_INSTANCE_FIELD(VideoMenuStatus*, menuStatus);
    DECLARE_INSTANCE_FIELD(bool, videoMenuInitialized);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::IPreviewBeatmapLevel*, currentLevel);
    DECLARE_INSTANCE_FIELD(bool, currentLevelIsPlaylistSong); // is this even needed on quest ?
    DECLARE_INSTANCE_FIELD(bool, videoMenuActive);
    DECLARE_INSTANCE_FIELD(int, selectedCell);

    static inline VideoMenu* instance;

public:

    static VideoMenu* get_instance();

    std::optional<ExtraSongData::ExtraSongData> extraSongData;
    std::optional<ExtraSongData::DifficultyData> difficultyData;
    std::optional<VideoConfig> currentVideo;
    std::string searchText;
    std::optional<std::string> thumbnailURL;

    std::shared_ptr<BSML::BSMLParserParams> bsmlParserParams;
    // download controller, search controller, result array

    DECLARE_DEFAULT_CTOR();

    DECLARE_INSTANCE_METHOD(void, Init);
    DECLARE_INSTANCE_METHOD(void, CreateStatusListener);
    DECLARE_INSTANCE_METHOD(void, AddTab);
    DECLARE_INSTANCE_METHOD(void, RemoveTab);
    DECLARE_INSTANCE_METHOD(void, ResetVideoMenu);
    DECLARE_INSTANCE_METHOD(void, SetButtonState, bool state);
    DECLARE_INSTANCE_METHOD(void, SetupVideoDetails);
    DECLARE_INSTANCE_METHOD(void, SetThumbnailFromCover, GlobalNamespace::IPreviewBeatmapLevel* level);
    DECLARE_INSTANCE_METHOD(void, SetSelectedLevel, GlobalNamespace::IPreviewBeatmapLevel* level);
    DECLARE_INSTANCE_METHOD(void, HandleDidSelectLevel, GlobalNamespace::IPreviewBeatmapLevel* level, bool isPlaylistSong = false);
    DECLARE_INSTANCE_METHOD(void, OnLevelSelected, GlobalNamespace::IPreviewBeatmapLevel* level);
    DECLARE_INSTANCE_METHOD(void, StatusViewerDidEnable);
    DECLARE_INSTANCE_METHOD(void, StatusViewerDidDisable);
    DECLARE_INSTANCE_METHOD(void, ApplyOffset, int offset);
    DECLARE_INSTANCE_METHOD(void, SearchAction);
    DECLARE_INSTANCE_METHOD(void, ShowKeyboard);
    DECLARE_INSTANCE_METHOD(void, OnRefineAction);
    DECLARE_INSTANCE_METHOD(void, OnDeleteVideoAction);
    DECLARE_INSTANCE_METHOD(void, OnDeleteConfigAction);
    DECLARE_INSTANCE_METHOD(void, OnBackAction);
    DECLARE_INSTANCE_METHOD(void, SearchFinished);
    DECLARE_INSTANCE_METHOD(void, ResetSearchView);
    DECLARE_INSTANCE_METHOD(void, OnQueryAction, StringW query);
    DECLARE_INSTANCE_METHOD(void, OnSelectCell, HMUI::TableView* view, int idx);
    DECLARE_INSTANCE_METHOD(void, OnDownloadAction);
    DECLARE_INSTANCE_METHOD(void, OnPreviewAction);
    DECLARE_INSTANCE_METHOD(void, DecreaseOffsetHigh);
    DECLARE_INSTANCE_METHOD(void, DecreaseOffsetMid);
    DECLARE_INSTANCE_METHOD(void, DecreaseOffsetLow);
    DECLARE_INSTANCE_METHOD(void, IncreaseOffsetHigh);
    DECLARE_INSTANCE_METHOD(void, IncreaseOffsetMid);
    DECLARE_INSTANCE_METHOD(void, IncreaseOffsetLow);

    DECLARE_INSTANCE_METHOD(void, set_CustomizeOffset, bool value);
    DECLARE_INSTANCE_METHOD(bool, get_CustomizeOffset);

public:

    void OnDownloadProgress(const VideoConfig& videoConfig);
    void CheckEntitlementAndEnableSearch(GlobalNamespace::IPreviewBeatmapLevel* level);
    void SetupLevelDetailView(const VideoConfig& videoConfig);
    void UpdateStatusText(const VideoConfig& videoConfig);
    void SetThumbnail(std::optional<std::string> url);
    void OnDifficultySelected(ExtraSongDataArgs extraSongDataArgs);
    void OnConfigChanged(const std::optional<VideoConfig> config);// might not be needed
//    custom_types::Helpers::Coroutine UpdateSearchResults()
    void OnDownloadFinished(const VideoConfig& video);
    // search progress
    custom_types::Helpers::Coroutine SearchLoadingCoroutine();

    __declspec(property(get=get_CustomizeOffset)) bool CustomizeOffset;

)