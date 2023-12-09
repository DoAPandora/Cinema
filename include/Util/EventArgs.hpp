#pragma once

#include "ExtraSongData.hpp"

#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"

#include <optional>
#include <string>
#include <utility>

namespace Cinema {

    struct LevelSelectedArgs {

        GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel;
        std::string originalPath;

        explicit LevelSelectedArgs(GlobalNamespace::IPreviewBeatmapLevel* level, std::string path = ""): previewBeatmapLevel(level), originalPath(std::move(path)) {}
    };

    struct ExtraSongDataArgs {

        std::optional<ExtraSongData::ExtraSongData> songData;
        std::optional<ExtraSongData::DifficultyData> selectedDifficultyData;

        ExtraSongDataArgs(std::optional<ExtraSongData::ExtraSongData> _songData, std::optional<ExtraSongData::DifficultyData> difficultyData): songData(std::move(_songData)), selectedDifficultyData(std::move(difficultyData)) {}
    };
}