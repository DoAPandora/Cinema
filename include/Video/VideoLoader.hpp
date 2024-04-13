#pragma once

#include "GlobalNamespace/BeatmapLevel.hpp"
#include "GlobalNamespace/AudioClipAsyncLoader.hpp"
#include "GlobalNamespace/BeatmapLevelsModel.hpp"
#include "GlobalNamespace/IAdditionalContentModel.hpp"
#include "GlobalNamespace/AsyncCache_2.hpp"
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

        GlobalNamespace::AsyncCache_2<StringW, GlobalNamespace::BeatmapLevel *>* get_BeatmapLevelAsyncCache();

        void Init();

        System::Collections::Generic::List_1<GlobalNamespace::BeatmapLevel *>* GetOfficialMaps();

        std::string GetConfigPath(GlobalNamespace::BeatmapLevel *level);

        std::string GetConfigPath(const std::string& levelPath);

        void AddConfigToCache(Cinema::VideoConfig &videoConfig, GlobalNamespace::BeatmapLevel *level);

        void RemoveConfigFromCache(GlobalNamespace::BeatmapLevel *level);

        VideoConfigPtr GetConfigFromCache(GlobalNamespace::BeatmapLevel *level);

        VideoConfigPtr GetConfigFromBundledConfigs(GlobalNamespace::BeatmapLevel *level);

        void StopFileSystemWatcher();

        void SetupFileSystemWatcher(GlobalNamespace::BeatmapLevel *level);

        void SetupFileSystemWatcher(std::string path);

        void ListenForConfigChanges(std::string levelPath);

        void OnConfigChanged();

        void OnConfigChangedMainThread();

        bool IsDlcSong(GlobalNamespace::BeatmapLevel *level);

        void GetAudioClipForLevel(GlobalNamespace::BeatmapLevel *level, const std::function<void(UnityW<UnityEngine::AudioClip>)> &callback);

        void LoadAudioClipAsync(GlobalNamespace::BeatmapLevel *level, const std::function<void(UnityW<UnityEngine::AudioClip>)> &callback);

        void GetEntitlementForLevel(GlobalNamespace::BeatmapLevel *level, const std::function<void(GlobalNamespace::EntitlementStatus)> &callback);

        VideoConfigPtr GetConfigForLevel(GlobalNamespace::BeatmapLevel *level);

        std::string GetLevelPath(GlobalNamespace::BeatmapLevel *level);

        void SaveVideoConfig(VideoConfigPtr);

        void SaveVideoConfigToPath(VideoConfigPtr videoConfig, std::string path);

        void DeleteVideo(VideoConfigPtr videoConfig);

        bool DeleteConfig(VideoConfigPtr videoConfig, GlobalNamespace::BeatmapLevel *level);

        VideoConfigPtr LoadConfig(std::string path);

        std::vector<BundledConfig> LoadBundledConfigs();
    }
}