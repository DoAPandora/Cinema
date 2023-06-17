#pragma once

#include "custom-types/shared/macros.hpp"
#include "Video/VideoConfig.hpp"

#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "HMUI/ImageView.hpp"
#include "questui/shared/CustomTypes/Components/ProgressBar/ProgressBar.hpp"

DECLARE_CLASS_CODEGEN(Cinema, VideoMenuManager, Il2CppObject,

    enum class DownloadState {
        Downloaded,
        Downloading,
        NotDownloaded
    };
    void SetDownloadState(DownloadState state);

    std::string ToDuration(int length);

    DECLARE_INSTANCE_METHOD(bool, DownloadCurrentVideo);
    DECLARE_INSTANCE_METHOD(void, DeleteCurrentVideo);
    DECLARE_INSTANCE_METHOD(void, DownloadCurrentThumbnail);
    DECLARE_INSTANCE_METHOD(void, SetCurrentThumbnail);
    DECLARE_INSTANCE_METHOD(void, UpdateMenu);
    DECLARE_INSTANCE_METHOD(bool, ShouldCreateScreen);
    DECLARE_INSTANCE_METHOD(void, UpdateProgressBar, float progress);
    DECLARE_INSTANCE_METHOD(void, DownloadButtonPressed);
    DECLARE_INSTANCE_METHOD(void, PreviewButtonPressed);

    DECLARE_INSTANCE_FIELD(QuestUI::ProgressBar*, progressBar);

    DECLARE_INSTANCE_FIELD(BSML::Backgroundable*, mainVertical);
    DECLARE_INSTANCE_FIELD(BSML::Backgroundable*, noCinemaVertical);
    
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, videoTitleText);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, downloadButton);

    DECLARE_INSTANCE_FIELD(HMUI::ImageView*, thumbnailSprite);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, authorText);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, durationText);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, downloadStateText);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, downloadVideoButton);

    DECLARE_INSTANCE_METHOD(void, DecreaseOffsetMajor);
    DECLARE_INSTANCE_METHOD(void, DecreaseOffsetNormal);
    DECLARE_INSTANCE_METHOD(void, DecreaseOffsetMinor);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, videoOffsetText);
    DECLARE_INSTANCE_METHOD(void, IncreaseOffsetMinor);
    DECLARE_INSTANCE_METHOD(void, IncreaseOffsetNormal);
    DECLARE_INSTANCE_METHOD(void, IncreaseOffsetMajor);

    DECLARE_CTOR(ctor);

    public:
        bool doesCurrentSongUseCinema;
        bool isCurrentVideoDownloaded;
        VideoConfig currentVideoConfig;
        enum DownloadState downloadState;

        static VideoMenuManager *get_instance();
    private:
        static SafePtr<VideoMenuManager> instance;
)