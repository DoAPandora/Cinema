#pragma once

#include "ExtraSongData.hpp"

#include <optional>
#include <string>
#include <utility>

namespace Cinema {

    struct ExtraSongDataArgs {

        std::optional<ExtraSongData::ExtraSongData> songData;
        std::optional<ExtraSongData::DifficultyData> selectedDifficultyData;

        ExtraSongDataArgs(std::optional<ExtraSongData::ExtraSongData> _songData, std::optional<ExtraSongData::DifficultyData> difficultyData): songData(std::move(_songData)), selectedDifficultyData(std::move(difficultyData)) {}
    };
}