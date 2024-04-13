#pragma once

#include "Util/ExtraSongData.hpp"

#include "GlobalNamespace/BeatmapKey.hpp"

#include <string>
#include <optional>

namespace Collections {

    std::optional<ExtraSongData::ExtraSongData> RetrieveExtraSongData(std::string hash);

    std::optional<ExtraSongData::DifficultyData> RetrieveDifficultyData(GlobalNamespace::BeatmapKey beatmapKey);
}