#include "main.hpp"
#include "Video/VideoLoader.hpp"
#include "Util/DelegateHelper.hpp"
#include "assets.hpp"
#include "Util/Util.hpp"

#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapLevelPackCollectionSO.hpp"
#include "GlobalNamespace/BeatmapLevelPack.hpp"
#include "GlobalNamespace/BeatmapLevelPackSO.hpp"
#include "GlobalNamespace/BeatmapLevelCollectionSO.hpp"
#include "GlobalNamespace/BeatmapLevelSO.hpp"
#include "GlobalNamespace/PreviewBeatmapLevelSO.hpp"
#include "GlobalNamespace/IBeatmapLevelData.hpp"
#include "GlobalNamespace/BeatmapLevelPackCollectionContainerSO.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Action_1.hpp"

#include "custom-types/shared/delegate.hpp"

#include <filesystem>

using namespace GlobalNamespace;

std::map<std::string, Cinema::VideoConfig> mapsWithVideo;
std::map<std::string, Cinema::VideoConfig> cachedConfigs;
std::map<std::string, Cinema::VideoConfig> bundledConfigs;

GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel;
AdditionalContentModel* additionalContentModel;
AudioClipAsyncLoader* audioClipAsyncLoader;

namespace Cinema::VideoLoader {

    GlobalNamespace::BeatmapLevelsModel *get_BeatmapLevelsModel() {
        if (!beatmapLevelsModel || !beatmapLevelsModel->m_CachedPtr.m_value) {
            beatmapLevelsModel = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::BeatmapLevelsModel *>().FirstOrDefault(
                    [](const auto &x) { return x->get_name()->Contains("(Clone)"); });
            if (!beatmapLevelsModel) {
                ERROR("Failed to get a reference to BeatmapLevelsModel");
            }
        }
        return beatmapLevelsModel;
    }
    __declspec(property(get=get_BeatmapLevelsModel)) GlobalNamespace::BeatmapLevelsModel *BeatmapLevelsModel;

    GlobalNamespace::AdditionalContentModel *get_AdditionalContentModel() {
        if (!additionalContentModel || !additionalContentModel->m_CachedPtr.m_value) {
            additionalContentModel = BeatmapLevelsModel->additionalContentModel;
            if (!additionalContentModel) {
                ERROR("Failed to get a reference to AdditionalContentModel");
            }
        }
        return additionalContentModel;
    }
    __declspec(property(get=get_AdditionalContentModel)) GlobalNamespace::AdditionalContentModel *AdditionalContentModel;

    GlobalNamespace::AudioClipAsyncLoader *get_AudioClipAsyncLoader() {
        if (!audioClipAsyncLoader) {
            audioClipAsyncLoader = BeatmapLevelsModel->audioClipAsyncLoader;
            if (!audioClipAsyncLoader) {
                ERROR("Failed to get a reference to AudioClipAsyncLoader");
            }
        }
        return audioClipAsyncLoader;
    }
    __declspec(property(get=get_AudioClipAsyncLoader)) GlobalNamespace::AudioClipAsyncLoader *AudioClipAsyncLoader;

    AsyncCache_2<StringW, IBeatmapLevel *> *get_BeatmapLevelAsyncCache() {
        return nullptr;
    }
    __declspec(property(get=get_BeatmapLevelAsyncCache)) GlobalNamespace::AsyncCache_2<StringW, GlobalNamespace::IBeatmapLevel *>* BeatmapLevelAsyncCache;

    void Init() {
        DEBUG("Loading bundled configs");
        auto configs = LoadBundledConfigs();
        for (const auto &config: configs) {
            bundledConfigs.insert_or_assign(config.levelID, config.config);
        }
    }

    List<IPreviewBeatmapLevel *> *GetOfficialMaps() {
        auto officialMaps = List<IPreviewBeatmapLevel*>::New_ctor();

        if (!BeatmapLevelsModel)
        {
            return officialMaps;
        }

        for(auto levelPack : BeatmapLevelsModel->ostAndExtrasPackCollection->beatmapLevelPacks)
        {
            officialMaps->AddRange(reinterpret_cast<System::Collections::Generic::IEnumerable_1<IPreviewBeatmapLevel*>*>(levelPack->beatmapLevelCollection->beatmapLevels->GetEnumerator()));
        }
        for(auto levelPack : BeatmapLevelsModel->dlcLevelPackCollectionContainer->beatmapLevelPackCollection->beatmapLevelPacks)
        {
            officialMaps->AddRange(reinterpret_cast<System::Collections::Generic::IEnumerable_1<IPreviewBeatmapLevel*>*>(levelPack->beatmapLevelCollection->beatmapLevels->GetEnumerator()));
        }

        return officialMaps;
    }

    std::string GetConfigPath(IPreviewBeatmapLevel *level) {
        auto levelPath = GetLevelPath(level);
        std::filesystem::path p(levelPath);
        return p / CONFIG_FILENAME;
    }

    std::string GetConfigPath(const std::string &levelPath) {
        std::filesystem::path p(levelPath);
        return p / CONFIG_FILENAME;
    }

    void AddConfigToCache(const VideoConfig &config, IPreviewBeatmapLevel *level) {
        auto success = cachedConfigs.insert({static_cast<std::string>(level->get_levelID()), config});
        if (success.second) {
            DEBUG("Adding config for {} to cache", level->get_levelID());
        }
    }

    void RemoveConfigFromCache(IPreviewBeatmapLevel *level) {
        auto success = cachedConfigs.erase(static_cast<std::string>(level->get_levelID()));
        if (success) {
            DEBUG("Removing config for {} from cache", level->get_levelID());
        }
    }

    std::optional<VideoConfig> GetConfigFromCache(IPreviewBeatmapLevel *level) {
        auto key = static_cast<std::string>(level->get_levelID());
        if (cachedConfigs.contains(key)) {
            DEBUG("Loading config for {} from cache", key);
            return cachedConfigs.at(key);
        }
        return std::nullopt;
    }

    std::optional<VideoConfig> GetConfigFromBundledConfigs(IPreviewBeatmapLevel *level) {
        auto cast = il2cpp_utils::try_cast<CustomPreviewBeatmapLevel>(level);
        auto levelID = cast.has_value() ? static_cast<std::string>(level->get_levelID())
                                        : Cinema::Util::ReplaceIllegalFilesystemChar(static_cast<std::string>(level->get_songName()->Trim()));

        if (!bundledConfigs.contains(levelID)) {
            DEBUG("No bundled config found for {}", levelID);
            return std::nullopt;
        }

        auto config = bundledConfigs.at(levelID);
        config.levelDir = GetLevelPath(level);
        config.bundledConfig = true;
        return config;
    }

    bool IsDlcSong(IPreviewBeatmapLevel *level) {
        return il2cpp_utils::try_cast<PreviewBeatmapLevelSO>(level).has_value();
    }

    void GetAudioClipForLevel(GlobalNamespace::IPreviewBeatmapLevel *level,
                              const std::function<void(UnityEngine::AudioClip *)> &callback) {
        if (!IsDlcSong(level) || !BeatmapLevelsModel) {
            return LoadAudioClipAsync(level, callback);
        }

        DelegateHelper::ContinueWith(BeatmapLevelAsyncCache->get_Item(level->get_levelID()),
                                     std::function([level, callback](IBeatmapLevel *levelData) {
                                         if (levelData) {
                                             DEBUG("Getting audio clip from async cache");
                                             return callback(levelData->get_beatmapLevelData()->get_audioClip());
                                         }

                                         LoadAudioClipAsync(level, callback);
                                     }));
    }

    void
    LoadAudioClipAsync(IPreviewBeatmapLevel *level, const std::function<void(UnityEngine::AudioClip *)> &callback) {
        auto loaderTask = AudioClipAsyncLoader->LoadPreview(level);
        if (!loaderTask) {
            ERROR("AudioClipAsyncLoader.LoadPreview() Failed");
            return;
        }

        DelegateHelper::ContinueWith(loaderTask, callback);
    }

    void GetEntitlementForLevel(IPreviewBeatmapLevel *level, const std::function<void(
            GlobalNamespace::AdditionalContentModel::EntitlementStatus)> &callback) {
        if (AdditionalContentModel) {
            return DelegateHelper::ContinueWith(
                    AdditionalContentModel->GetLevelEntitlementStatusAsync(level->get_levelID(),
                                                                           System::Threading::CancellationToken::get_None()),
                    callback);
        }
        callback(AdditionalContentModel::EntitlementStatus::Owned);
    }

    std::optional<VideoConfig> GetConfigForLevel(IPreviewBeatmapLevel *level) {
        auto cachedConfig = GetConfigFromCache(level);
        if (cachedConfig.has_value()) {
            if (cachedConfig->downloadState == DownloadState::Downloaded) {
                RemoveConfigFromCache(level);
                return cachedConfig;
            }
        }

        std::optional<VideoConfig> videoConfig = std::nullopt;
        auto levelPath = GetLevelPath(level);
        if (std::filesystem::exists(levelPath)) {
            videoConfig = LoadConfig(GetConfigPath(levelPath));
        } else {
            DEBUG("Path does not exist: {}", levelPath);
        }

        if (videoConfig == std::nullopt) {
            videoConfig = GetConfigFromBundledConfigs(level);
            if (videoConfig == std::nullopt) {
                return videoConfig;
            }
            DEBUG("Loaded from bundled configs");
        }
        return videoConfig;
    }

    std::string GetLevelPath(IPreviewBeatmapLevel *level) {
        auto cast = il2cpp_utils::try_cast<CustomPreviewBeatmapLevel>(level);
        if (cast.has_value()) {
            return static_cast<std::string>(cast.value()->customLevelPath);
        }

        auto songName = static_cast<std::string>(level->get_songName()->Trim());
        std::filesystem::path p("/sdcard/ModData/com.beatgames.beatsaber/Mods/SongLoader/CustomLevels");
        return p / OST_DIRECTORY_NAME / Cinema::Util::ReplaceIllegalFilesystemChar(songName);
    }

    void SaveVideoConfig(VideoConfig &videoConfig) {
        if (videoConfig.levelDir == std::nullopt || videoConfig.ConfigPath == std::nullopt ||
            !std::filesystem::exists(*videoConfig.levelDir)) {
            WARN("Failed to save video. Path {} does not exist.", *videoConfig.levelDir);
            return;
        }

        auto configPath = *videoConfig.ConfigPath;
        SaveVideoConfigToPath(videoConfig, configPath);
    }

    void SaveVideoConfigToPath(VideoConfig &config, std::string path) {
        INFO("Saving video config to {}", path);

        try {
            WriteToFile(path, config);
            config.needsToSave = false;
        }
        catch (std::exception &e) {
            ERROR("Failed to save level data:\n{}", e.what());
        }

        if (!std::filesystem::exists(path)) {
            ERROR("Config file doesn't exist after saving!");
        } else {
            DEBUG("Config save successful");
        }
    }

    void DeleteVideo(VideoConfig& videoConfig)
    {
        if (videoConfig.VideoPath == std::nullopt)
        {
            WARN("Tried to delete video, but its path was null");
            return;
        }

        try
        {
            std::filesystem::remove(*videoConfig.VideoPath);
            INFO("Deleted video at {}", videoConfig.VideoPath);
            if(videoConfig.downloadState != DownloadState::Cancelled)
            {
                videoConfig.downloadState = DownloadState::NotDownloaded;
            }

            videoConfig.videoFile = std::nullopt;
        }
        catch (const std::exception& e)
        {
            ERROR("Failed to delete video at {}\n{}", videoConfig.VideoPath, e.what());
        }
    }

    bool DeleteConfig(Cinema::VideoConfig &videoConfig, GlobalNamespace::IPreviewBeatmapLevel *level)
    {
        if (videoConfig.levelDir == std::nullopt)
        {
            ERROR("levelDir was null when trying to delete config");
            return false;
        }

        try
        {
            auto cinemaConfigPath = GetConfigPath(*videoConfig.levelDir);
            if (std::filesystem::exists(cinemaConfigPath))
            {
                std::filesystem::remove(cinemaConfigPath);
            }

            auto key = static_cast<std::string>(level->get_levelID());
            if (mapsWithVideo.contains(key))
            {
                mapsWithVideo.erase(key);
            }
        }
        catch (const std::exception& e)
        {
            ERROR("Failed to delete video config:\n{}", e.what());
        }

        RemoveConfigFromCache(level);
        INFO("Deleted video config");
        return true;
    }

    std::optional<VideoConfig> LoadConfig(std::string configPath)
    {
        if (!std::filesystem::exists(configPath))
        {
            return std::nullopt;
        }

        std::optional<VideoConfig> videoConfig;
        try
        {
            VideoConfig c;
            ReadFromFile(configPath, c);
            videoConfig = c;
        }
        catch (const std::exception& e)
        {
            ERROR("Error parsing video json {}:\n{}", configPath, e.what());
            return std::nullopt;
        }

        if(videoConfig != std::nullopt)
        {
            videoConfig->levelDir = std::filesystem::path(configPath).string();
            videoConfig->UpdateDownloadState();
        }
        else
        {
            WARN("Deserializing video config at {} failed", configPath);
        }

        return videoConfig;
    }

    std::vector<BundledConfig> LoadBundledConfigs()
    {
        DEBUG("Loading bundled configs");
        std::string_view configs = IncludedAssets::configs_json;
        std::vector<BundledConfig> bundledConfigs;

        rapidjson::Document doc;

        doc.Parse(configs.data());
        auto array = doc.GetArray();
        for(auto & itr : array)
        {
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            itr.Accept(writer);

            BundledConfig c;
            bundledConfigs.push_back(c);
        }
        return bundledConfigs;
    }
}