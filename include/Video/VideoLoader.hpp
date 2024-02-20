#pragma once

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/AudioClipAsyncLoader.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/IAdditionalContentModel.hpp"
#include "GlobalNamespace/AsyncCache_2.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/EntitlementsStatus.hpp"

#include "System/Collections/Generic/List_1.hpp"

#include "UnityEngine/AudioClip.hpp"

#include "Video/VideoConfig.hpp"

#define OST_DIRECTORY_NAME "CinemaOSTVideos"
#define WIP_DIRECTORY_NAME "CinemaWIPVideos"
#define WIP_MAPS_FOLDER "CustomWIPLevels"
#define CONFIG_FILENAME "cinema-video.json"
#define CONFIG_FILENAME_MVP "video.json"
#define MOD_ID_MVP "Music Video Player"

namespace Cinema {

    namespace VideoLoader {

        extern UnorderedEventCallback<Cinema::VideoConfig &> configChanged;

        GlobalNamespace::BeatmapLevelsModel *get_BeatmapLevelsModel();

        GlobalNamespace::IAdditionalContentModel *get_AdditionalContentModel();

        GlobalNamespace::AudioClipAsyncLoader *get_AudioClipAsyncLoader();

        GlobalNamespace::AsyncCache_2<StringW, GlobalNamespace::IBeatmapLevel *>* get_BeatmapLevelAsyncCache();

        void Init();

        System::Collections::Generic::List_1<GlobalNamespace::IPreviewBeatmapLevel *>* GetOfficialMaps();

        std::string GetConfigPath(GlobalNamespace::IPreviewBeatmapLevel *level);

        std::string GetConfigPath(const std::string& levelPath);

        void AddConfigToCache(Cinema::VideoConfig &videoConfig, GlobalNamespace::IPreviewBeatmapLevel *level);

        void RemoveConfigFromCache(GlobalNamespace::IPreviewBeatmapLevel *level);

        std::optional<Cinema::VideoConfig> GetConfigFromCache(GlobalNamespace::IPreviewBeatmapLevel *level);

        std::optional<Cinema::VideoConfig> GetConfigFromBundledConfigs(GlobalNamespace::IPreviewBeatmapLevel *level);

        void StopFileSystemWatcher();

        void SetupFileSystemWatcher(GlobalNamespace::IPreviewBeatmapLevel *level);

        void SetupFileSystemWatcher(std::string path);

        void ListenForConfigChanges(std::string levelPath);

        void OnConfigChanged();

        void OnConfigChangedMainThread();

        bool IsDlcSong(GlobalNamespace::IPreviewBeatmapLevel *level);

        void GetAudioClipForLevel(GlobalNamespace::IPreviewBeatmapLevel *level, const std::function<void(UnityW<UnityEngine::AudioClip>)> &callback);

        void LoadAudioClipAsync(GlobalNamespace::IPreviewBeatmapLevel *level, const std::function<void(UnityW<UnityEngine::AudioClip>)> &callback);

        void GetEntitlementForLevel(GlobalNamespace::IPreviewBeatmapLevel *level, const std::function<void(GlobalNamespace::EntitlementStatus)> &callback);

        std::optional<Cinema::VideoConfig> GetConfigForLevel(GlobalNamespace::IPreviewBeatmapLevel *level);

        std::string GetLevelPath(GlobalNamespace::IPreviewBeatmapLevel *level);

        void SaveVideoConfig(Cinema::VideoConfig &videoConfig);

        void SaveVideoConfigToPath(Cinema::VideoConfig &videoConfig, std::string path);

        void DeleteVideo(Cinema::VideoConfig &videoConfig);

        bool DeleteConfig(Cinema::VideoConfig &videoConfig, GlobalNamespace::IPreviewBeatmapLevel *level);

        std::optional<VideoConfig> LoadConfig(std::string path);

        std::vector<BundledConfig> LoadBundledConfigs();
    }
}