#include "Util/Collections.hpp"
#include "main.hpp"

#include "System/Collections/Generic/IEnumerable_1.hpp"
#include "System/Collections/Generic/IEnumerator_1.hpp"

#include "songcore/shared/SongLoader/RuntimeSongLoader.hpp"

#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"

#include <map>
#include <ranges>
#include <vector>

using namespace SongCore::SongLoader;

static std::map<std::string, ExtraSongData::ExtraSongData> extraSongDataCache;

namespace Collections
{

    std::optional<ExtraSongData::ExtraSongData> RetrieveExtraSongData(std::string hash)
    {

        if(hash.starts_with("custom_level_"))
            hash = hash.substr(13);

        std::transform(hash.cbegin(), hash.cend(), hash.begin(), toupper);

        if(extraSongDataCache.contains(hash))
            return extraSongDataCache.at(hash);

        auto customLevels = RuntimeSongLoader::get_instance()->AllLevels;
        if(customLevels.size() <= 0)
        {
            return std::nullopt;
        }

        auto beatmapIter = std::ranges::find_if(customLevels, [&](CustomBeatmapLevel* level)
                                                { return level->levelID.ends_with(hash); });
        if(beatmapIter == customLevels.end())
        {
            return std::nullopt;
        }

        CustomBeatmapLevel* level = *beatmapIter;
        std::vector<ExtraSongData::DifficultyData> difficulties;

        auto enumerator_1 = level->GetBeatmapKeys()->GetEnumerator();
        auto enumerator = enumerator_1->i___System__Collections__IEnumerator();
        while(enumerator->MoveNext())
        {
            auto key = enumerator_1->Current;
            auto saveData = level->CustomSaveDataInfo;
            if(!saveData)
            {
                continue;
            }

            auto& difficultyData = saveData.value().get().TryGetCharacteristic(key.beatmapCharacteristic->serializedName)->get().TryGetDifficulty(key.difficulty)->get();

            difficulties.emplace_back(
                difficultyData.requirements,
                difficultyData.suggestions,
                difficultyData.characteristicName,
                difficultyData.difficulty);
        }

        auto extraSongData = ExtraSongData::ExtraSongData(difficulties);
        extraSongDataCache.insert({hash, extraSongData});

        return extraSongData;
    }

    std::optional<ExtraSongData::DifficultyData> RetrieveDifficultyData(GlobalNamespace::BeatmapKey beatmapKey)
    {
        auto songData = RetrieveExtraSongData(beatmapKey.levelId);
        if(!songData)
        {
            return std::nullopt;
        }
        auto result = std::ranges::find_if(songData->difficulties, [&beatmapKey](ExtraSongData::DifficultyData diff)
                                           { return diff.difficulty == beatmapKey.difficulty &&
                                                    (diff.beatmapCharacteristicName == beatmapKey.beatmapCharacteristic->serializedName ||
                                                     diff.beatmapCharacteristicName == beatmapKey.beatmapCharacteristic->characteristicNameLocalizationKey); });

        if(result != songData->difficulties.end())
            return *result;
        return std::nullopt;
    }
} // namespace Collections