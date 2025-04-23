#include "Video/VideoLoader.hpp"
#include "Util/DelegateHelper.hpp"
#include "Util/Util.hpp"
#include "assets.hpp"
#include "main.hpp"

#include "GlobalNamespace/AudioClipAsyncLoaderExtensions.hpp"
#include "GlobalNamespace/BeatmapLevelCollectionSO.hpp"
#include "GlobalNamespace/BeatmapLevelLoader.hpp"
#include "GlobalNamespace/BeatmapLevelPack.hpp"
#include "GlobalNamespace/BeatmapLevelPackSO.hpp"
#include "GlobalNamespace/BeatmapLevelSO.hpp"
#include "GlobalNamespace/IAdditionalContentEntitlementModel.hpp"
#include "GlobalNamespace/IBeatmapLevelData.hpp"
#include "GlobalNamespace/IPreviewMediaData.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"

#include "songcore/shared/SongLoader/CustomBeatmapLevel.hpp"

#include "UnityEngine/Resources.hpp"

#include "System/Action_1.hpp"
#include "System/Collections/Generic/IReadOnlyList_1.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "System/Threading/Tasks/Task_1.hpp"

#include "bsml/shared/Helpers/getters.hpp"

#include <filesystem>

using namespace GlobalNamespace;

using BGLib::DotnetExtension::Collections::LRUCache_2;

template <typename T>
using Task = System::Threading::Tasks::Task_1<T>;

using System::Collections::Generic::List_1;

std::map<std::string, std::shared_ptr<Cinema::VideoConfig>> mapsWithVideo;
std::map<std::string, std::shared_ptr<Cinema::VideoConfig>> cachedConfigs;
std::map<std::string, std::shared_ptr<Cinema::VideoConfig>> bundledConfigs;

GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel;
IAdditionalContentModel* additionalContentModel;
AudioClipAsyncLoader* audioClipAsyncLoader;
IAdditionalContentEntitlementModel* additionalContentEntitlementModel;
LRUCache_2<StringW, IBeatmapLevelData*>* beatmapLevelDataCache;

namespace Cinema::VideoLoader
{

    Zenject::DiContainer* get_DiContainer()
    {
        return BSML::Helpers::GetDiContainer();
    }
    __declspec(property(get = get_DiContainer)) Zenject::DiContainer* DiContainer;

    GlobalNamespace::BeatmapLevelsModel* get_BeatmapLevelsModel()
    {
        if(!beatmapLevelsModel)
        {
            beatmapLevelsModel = DiContainer->Resolve<BeatmapLevelsModel*>();
            if(!beatmapLevelsModel)
            {
                ERROR("Failed to get a reference to BeatmapLevelsModel");
            }
        }
        return beatmapLevelsModel;
    }

    GlobalNamespace::IAdditionalContentModel* get_AdditionalContentModel()
    {
        if(!additionalContentModel)
        {
            additionalContentModel = DiContainer->Resolve<IAdditionalContentModel*>();
            if(!additionalContentModel)
            {
                ERROR("Failed to get a reference to AdditionalContentModel");
            }
        }
        return additionalContentModel;
    }

    GlobalNamespace::AudioClipAsyncLoader* get_AudioClipAsyncLoader()
    {
        if(!audioClipAsyncLoader)
        {
            audioClipAsyncLoader = DiContainer->Resolve<AudioClipAsyncLoader*>();
            if(!audioClipAsyncLoader)
            {
                ERROR("Failed to get a reference to AudioClipAsyncLoader");
            }
        }
        return audioClipAsyncLoader;
    }

    IAdditionalContentEntitlementModel* get_AdditionalContentEntitlementModel()
    {
        if(!additionalContentEntitlementModel)
        {
            additionalContentEntitlementModel = DiContainer->Resolve<IAdditionalContentEntitlementModel*>();
            if(!additionalContentEntitlementModel)
            {
                ERROR("Failed to get a reference to AdditionalContentEntitlementModel");
            }
        }
        return additionalContentEntitlementModel;
    }

    LRUCache_2<StringW, IBeatmapLevelData*>* get_BeatmapLevelDataCache()
    {
        auto levelLoader = get_DiContainer()->Resolve<BeatmapLevelLoader*>();
        if(levelLoader)
        {
            beatmapLevelDataCache = levelLoader->_loadedBeatmapLevelDataCache;
        }

        return beatmapLevelDataCache;
    }

    void Init()
    {
        DEBUG("Loading bundled configs");
        auto configs = LoadBundledConfigs();
        for(const auto& config : configs)
        {
            VideoConfig videoConfig = config.config;
            bundledConfigs.insert_or_assign(config.levelID, std::make_shared<VideoConfig>(videoConfig));
        }
    }

    List_1<BeatmapLevel*>* GetOfficialMaps()
    {
        auto officialMaps = List_1<BeatmapLevel*>::New_ctor();

        if(!get_BeatmapLevelsModel())
        {
            return officialMaps;
        }

        using BeatmapLevelsEnumerable = System::Collections::Generic::IEnumerable_1<BeatmapLevel*>;
        ListW<BeatmapLevelPack*> list;
        // list.insert_range(BeatmapLevelsModel->ostAndExtrasBeatmapLevelsRepository->beatmapLevelPacks);
        for(BeatmapLevelPack* levelPack : list)
        {
            officialMaps->AddRange(reinterpret_cast<BeatmapLevelsEnumerable*>(levelPack->_beatmapLevels->GetEnumerator()));
        }
        list.clear();
        // list.insert_range(BeatmapLevelsModel->dlcBeatmapLevelsRepository->beatmapLevelPacks);
        for(BeatmapLevelPack* levelPack : list)
        {
            officialMaps->AddRange(reinterpret_cast<BeatmapLevelsEnumerable*>(levelPack->_beatmapLevels->GetEnumerator()));
        }

        return officialMaps;
    }

    std::string GetConfigPath(BeatmapLevel* level)
    {
        auto levelPath = GetLevelPath(level);
        std::filesystem::path p(levelPath);
        return p / CONFIG_FILENAME;
    }

    std::string GetConfigPath(const std::string& levelPath)
    {
        std::filesystem::path p(levelPath);
        return p / CONFIG_FILENAME;
    }

    void AddConfigToCache(std::shared_ptr<VideoConfig> config, BeatmapLevel* level)
    {
        if(config == nullptr)
        {
            return;
        }
        auto success = cachedConfigs.emplace(static_cast<std::string>(level->levelID), config);
        if(success.second)
        {
            DEBUG("Adding config for {} to cache", level->levelID);
        }
    }

    void RemoveConfigFromCache(BeatmapLevel* level)
    {
        auto success = cachedConfigs.erase(static_cast<std::string>(level->levelID));
        if(success)
        {
            DEBUG("Removing config for {} from cache", level->levelID);
        }
    }

    std::shared_ptr<VideoConfig> GetConfigFromCache(BeatmapLevel* level)
    {
        auto key = static_cast<std::string>(level->levelID);
        if(cachedConfigs.contains(key))
        {
            DEBUG("Loading config for {} from cache", key);
            return cachedConfigs.at(key);
        }
        return nullptr;
    }

    std::shared_ptr<VideoConfig> GetConfigFromBundledConfigs(BeatmapLevel* level)
    {
        auto cast = il2cpp_utils::try_cast<SongCore::SongLoader::CustomBeatmapLevel>(level);
        auto levelID = cast.has_value() ? static_cast<std::string>(level->levelID)
                                        : Cinema::Util::ReplaceIllegalFilesystemChar(static_cast<std::string>(level->songName->Trim()));

        if(!bundledConfigs.contains(levelID))
        {
            DEBUG("No bundled config found for {}", levelID);
            return nullptr;
        }

        auto config = bundledConfigs.at(levelID);
        config->levelDir = GetLevelPath(level);
        config->bundledConfig = true;
        return config;
    }

    bool IsDlcSong(BeatmapLevel* level)
    {
        // Is this correct ?? idk
        return il2cpp_utils::try_cast<BeatmapLevelSO>(level).has_value();
    }

    System::Threading::Tasks::Task_1<UnityW<UnityEngine::AudioClip>>* GetAudioClipForLevel(GlobalNamespace::BeatmapLevel* level)
    {
        // if (!IsDlcSong(level) || !get_BeatmapLevelAsyncCache())
        // {
        //     return LoadAudioClipAsync(level, callback);
        // }

        IBeatmapLevelData* levelData;
        if(get_BeatmapLevelDataCache()->TryGetFromCache(level->levelID, levelData))
        {
            return AudioClipAsyncLoaderExtensions::LoadSong(get_AudioClipAsyncLoader(), levelData);
        }

        return LoadAudioClipAsync(level);
    }

    Task<UnityW<UnityEngine::AudioClip>>* LoadAudioClipAsync(BeatmapLevel* level)
    {
        auto loadTask = AudioClipAsyncLoaderExtensions::LoadPreview(get_AudioClipAsyncLoader(), level);

        if(!loadTask)
        {
            ERROR("VideoLoader::LoadAudioClipAsync() failed");
            return Task<UnityW<UnityEngine::AudioClip>>::FromResult(UnityW<UnityEngine::AudioClip>(nullptr));
        }

        return loadTask;
    }

    Task<EntitlementStatus>* GetEntitlementForLevel(BeatmapLevel* level)
    {
        if(auto contentModel = get_AdditionalContentEntitlementModel())
        {
            return contentModel->GetLevelEntitlementStatusAsync(level->levelID, System::Threading::CancellationToken::get_None());
        }

        return Task<EntitlementStatus>::FromResult(EntitlementStatus::Owned);
    }

    std::shared_ptr<VideoConfig> GetConfigForLevel(BeatmapLevel* level)
    {
        auto cachedConfig = GetConfigFromCache(level);
        if(cachedConfig != nullptr)
        {
            if(cachedConfig->downloadState == DownloadState::Downloaded)
            {
                RemoveConfigFromCache(level);
                return cachedConfig;
            }
        }

        std::shared_ptr<VideoConfig> videoConfig;
        auto levelPath = GetLevelPath(level);
        if(std::filesystem::exists(levelPath))
        {
            videoConfig = LoadConfig(GetConfigPath(levelPath));
        }
        else
        {
            DEBUG("Path does not exist: {}", levelPath);
        }

        if(videoConfig == nullptr)
        {
            videoConfig = GetConfigFromBundledConfigs(level);
            if(videoConfig == nullptr)
            {
                return videoConfig;
            }
            DEBUG("Loaded from bundled configs");
        }
        return videoConfig;
    }

    std::string GetLevelPath(BeatmapLevel* level)
    {
        auto cast = il2cpp_utils::try_cast<SongCore::SongLoader::CustomBeatmapLevel>(level);
        if(cast.has_value())
        {
            return static_cast<std::string>(cast.value()->customLevelPath);
        }

        auto songName = static_cast<std::string>(level->songName->Trim());
        std::filesystem::path p("/sdcard/ModData/com.beatgames.beatsaber/Mods/SongLoader/CustomLevels");
        return p / OST_DIRECTORY_NAME / Cinema::Util::ReplaceIllegalFilesystemChar(songName);
    }

    void SaveVideoConfig(std::shared_ptr<VideoConfig> videoConfig)
    {
        if(!videoConfig->levelDir.has_value() || !videoConfig->ConfigPath.has_value() ||
           !std::filesystem::exists(videoConfig->levelDir.value()))
        {
            WARN("Failed to save video. Path {} does not exist.", videoConfig->levelDir.value());
            return;
        }

        auto configPath = videoConfig->ConfigPath.value();
        SaveVideoConfigToPath(videoConfig, configPath);
    }

    void SaveVideoConfigToPath(std::shared_ptr<VideoConfig> config, std::string path)
    {
        INFO("Saving video config to {}", path);

        try
        {
            WriteToFile(path, *config);
            config->needsToSave = false;
        } catch(std::exception& e)
        {
            ERROR("Failed to save level data:\n{}", e.what());
        }

        if(!std::filesystem::exists(path))
        {
            ERROR("Config file doesn't exist after saving!");
        }
        else
        {
            DEBUG("Config save successful");
        }
    }

    void DeleteVideo(std::shared_ptr<VideoConfig> videoConfig)
    {
        if(!videoConfig->VideoPath.has_value())
        {
            WARN("Tried to delete video, but its path was null");
            return;
        }

        try
        {
            std::filesystem::remove(videoConfig->VideoPath.value());
            INFO("Deleted video at {}", videoConfig->VideoPath);
            if(videoConfig->downloadState != DownloadState::Cancelled)
            {
                videoConfig->downloadState = DownloadState::NotDownloaded;
            }

            videoConfig->videoFile = std::nullopt;
        } catch(const std::exception& e)
        {
            ERROR("Failed to delete video at {}\n{}", videoConfig->VideoPath, e.what());
        }
    }

    bool DeleteConfig(std::shared_ptr<VideoConfig> videoConfig, GlobalNamespace::BeatmapLevel* level)
    {
        if(!videoConfig->levelDir.has_value())
        {
            ERROR("levelDir was null when trying to delete config");
            return false;
        }

        try
        {
            auto cinemaConfigPath = GetConfigPath(videoConfig->levelDir.value());
            if(std::filesystem::exists(cinemaConfigPath))
            {
                std::filesystem::remove(cinemaConfigPath);
            }

            auto key = static_cast<std::string>(level->levelID);
            if(mapsWithVideo.contains(key))
            {
                mapsWithVideo.erase(key);
            }
        } catch(const std::exception& e)
        {
            ERROR("Failed to delete video config:\n{}", e.what());
        }

        RemoveConfigFromCache(level);
        INFO("Deleted video config");
        return true;
    }

    std::shared_ptr<VideoConfig> LoadConfig(std::string configPath)
    {
        if(!std::filesystem::exists(configPath))
        {
            return nullptr;
        }

        VideoConfig videoConfig;
        try
        {
            ReadFromFile(configPath, videoConfig);
        } catch(const std::exception& e)
        {
            ERROR("Error parsing video json {}:\n{}", configPath, e.what());
            return nullptr;
        }

        videoConfig.levelDir = std::filesystem::path(configPath).parent_path().string();
        videoConfig.UpdateDownloadState();

        return std::make_shared<VideoConfig>(videoConfig);
    }

    std::vector<BundledConfig> LoadBundledConfigs()
    {
        DEBUG("Loading bundled configs");
        std::string_view configs = IncludedAssets::configs_json;
        std::vector<BundledConfig> bundledConfigs;

        rapidjson::Document doc;

        doc.Parse(configs.data());
        auto array = doc.GetArray();
        for(auto& itr : array)
        {
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            itr.Accept(writer);

            BundledConfig c;
            bundledConfigs.push_back(c);
        }
        return bundledConfigs;
    }
} // namespace Cinema::VideoLoader