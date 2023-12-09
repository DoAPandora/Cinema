#include "main.hpp"
#include "Util/Collections.hpp"

#include "songloader/shared/API.hpp"

#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/DifficultyBeatmapSet.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"

#include "rapidjson-macros/shared/macros.hpp"

#include <vector>
#include <map>
#include <locale>

static std::map<std::string, ExtraSongData::ExtraSongData> extraSongDataCache;

namespace Collections {

    template <bool b, typename T>
    std::vector<std::u16string> GetStringArray(const rapidjson::GenericObject<b, T>& obj, const char16_t* name)
    {
        if (!obj.HasMember(name))
        {
            return {};
        }

        std::vector<std::u16string> v;
        auto array = obj[name].GetArray();
        for( auto itr = array.begin(); itr != array.end(); itr++)
        {
            v.push_back(itr->GetString());
        }
        return v;
    }

    std::vector<ExtraSongData::DifficultyData> GetDifficultiesFromInfoDat(CustomJSONData::CustomLevelInfoSaveData* infoDat)
    {
        const auto& doc = *infoDat->doc;
        std::vector<ExtraSongData::DifficultyData> difficulties;

        auto difficultyBeatmapSets = doc[u"_difficultyBeatmapSets"].GetArray();
        for(const auto & difficultyBeatmapSet : difficultyBeatmapSets)
        {
            auto beatmapSet = difficultyBeatmapSet.GetObj();
            auto difficultyBeatmaps = beatmapSet[u"_difficultyBeatmaps"].GetArray();
            auto characteristic = beatmapSet[u"_beatmapCharacteristicName"].GetString();

            for(const auto & difficultyBeatmap : difficultyBeatmaps)
            {
                auto beatmap = difficultyBeatmap.GetObj();
                std::vector<std::u16string> suggestions;
                std::vector<std::u16string> requirements;

                if (beatmap.HasMember(u"_customData"))
                {
                    auto customData = beatmap[u"_customData"].GetObj();
                    suggestions = GetStringArray(customData, u"_suggestions");
                    requirements = GetStringArray(customData, u"_requirements");
                }

                auto difficultyRank = beatmap[u"_difficultyRank"].GetInt();
                difficultyRank = (difficultyRank - 1) / 2;

                difficulties.push_back(ExtraSongData::DifficultyData(requirements, suggestions, characteristic, difficultyRank));
            }
        }

        return difficulties;
    }

    std::optional<ExtraSongData::ExtraSongData> RetrieveExtraSongData(std::string hash) {

        if (hash.starts_with("custom_level_"))
            hash = hash.substr(13);

        std::transform(hash.cbegin(), hash.cend(), hash.begin(), toupper);

        if (extraSongDataCache.contains(hash))
            return extraSongDataCache.at(hash);

        auto customLevels = RuntimeSongLoader::API::GetCustomLevelsPack()->GetCustomPreviewBeatmapLevels();
        if(!customLevels)
        {
            return std::nullopt;
        }
        if (customLevels.Length() <= 0)
        {
            return std::nullopt;
        }

        auto beatmap = customLevels.FirstOrDefault([&hash](const auto& level) { return level->levelID.ends_with(hash);});
        if (!beatmap)
        {
            return std::nullopt;
        }

        auto infoDat = reinterpret_cast<CustomJSONData::CustomLevelInfoSaveData*>(beatmap->standardLevelInfoSaveData);
        auto difficulties = GetDifficultiesFromInfoDat(infoDat);

        auto extraSongData = ExtraSongData::ExtraSongData(difficulties);
        extraSongDataCache.insert({hash, extraSongData});

        return extraSongData;
    }

    std::optional<ExtraSongData::DifficultyData> RetrieveDifficultyData(GlobalNamespace::IDifficultyBeatmap* beatmap)
    {
        auto songData = RetrieveExtraSongData(beatmap->get_level()->i_IPreviewBeatmapLevel()->get_levelID());
        auto result = std::find_if(songData->difficulties.begin(), songData->difficulties.end(), [&beatmap](const auto& diff)
        {
            return diff.difficulty == beatmap->get_difficulty() &&
            (diff.beatmapCharacteristicName == beatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->serializedName ||
            diff.beatmapCharacteristicName == beatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->characteristicNameLocalizationKey);
        });

        if(result != songData->difficulties.end())
            return *result;
        return std::nullopt;
    }
}