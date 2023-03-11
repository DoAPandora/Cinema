#pragma once

#include "custom-types/shared/macros.hpp"
#include "json.hpp"

#include "HMUI/CurvedTextMeshPro.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/Sprite.hpp"
#include "questui/shared/CustomTypes/Components/ProgressBar/ProgressBar.hpp"

#define DECLARE_GET_SET(type, name) \
type get_##name(); \
void set_##name(type value);

DECLARE_CLASS_CODEGEN(Cinema, VideoMenuManager, Il2CppObject,

    bool DownloadVideo(std::string_view url, std::function<void(float)> status);
    std::string GetCurrentVideoPath();

    DECLARE_INSTANCE_METHOD(bool, DownloadCurrentVideo);
    DECLARE_INSTANCE_METHOD(void, UpdateMenu);
    DECLARE_INSTANCE_METHOD(bool, ShouldCreateScreen);
    DECLARE_INSTANCE_METHOD(void, UpdateProgressBar, float progress);
    DECLARE_INSTANCE_METHOD(void, DownloadButtonClicked);

    DECLARE_INSTANCE_FIELD(QuestUI::ProgressBar*, progressBar);

    
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, videoTitleText);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, downloadButton);

    DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, thumbnailSprite);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, authorText);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, durationText);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, downloadStateText);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, downloadVideoButton);

    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, minorDecreaseButton);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, normalDecreaseButton);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, majorDecreaseButton);
    DECLARE_INSTANCE_FIELD(HMUI::CurvedTextMeshPro*, videoOffsetText);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, minorIncreaseButton);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, normalIncreaseButton);
    DECLARE_INSTANCE_FIELD(UnityEngine::UI::Button*, majorIncreaseButton);


    DECLARE_GET_SET(bool, doesCurrentSongUseCinema);
    DECLARE_GET_SET(bool, isCurrentSongDownloaded);
    DECLARE_GET_SET(JSON::CinemaInfo, currentLevelData);

    DECLARE_CTOR(ctor);
    
    public:
        static VideoMenuManager *get_instance();
    private:
        static SafePtr<VideoMenuManager> instance;
        
        bool doesCurrentSongUseCinema;
        bool isCurrentSongDownloaded;
        JSON::CinemaInfo currentLevelData;
)