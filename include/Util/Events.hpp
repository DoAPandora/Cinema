#pragma once

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

#include "GlobalNamespace/BeatmapLevel.hpp"

#include "ExtraSongData.hpp"
#include "Video/VideoConfig.hpp"
#include "EventArgs.hpp"

namespace Cinema::Events {

    extern UnorderedEventCallback<bool> cinemaActivated;

    // No need for LevelSelectedArgs as beatmap editor is not on quest so there is only one source for level selection
    extern UnorderedEventCallback<GlobalNamespace::BeatmapLevel*> levelSelected;

    extern UnorderedEventCallback<ExtraSongDataArgs> difficultySelected;

    void InvokeSceneTransitionEvents(std::optional<VideoConfig> videoConfig);

    void SetSelectedLevel(GlobalNamespace::BeatmapLevel* level);

    void SetExtraSongData(const ExtraSongData::ExtraSongData& songData, const ExtraSongData::DifficultyData& selectedDifficultyData);
}