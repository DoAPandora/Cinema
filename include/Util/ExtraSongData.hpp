#pragma once

#include "GlobalNamespace/BeatmapDifficulty.hpp"

#include <vector>
#include <string>

namespace ExtraSongData {

    struct DifficultyData {

        std::vector<std::u16string> requirements;
        std::vector<std::u16string> suggestions;
        std::u16string beatmapCharacteristicName;
        GlobalNamespace::BeatmapDifficulty difficulty;

        inline bool HasCinemaSuggestion() const {
            auto iter = std::find(suggestions.begin(), suggestions.end(), u"Cinema");
            return iter != suggestions.end();
        }

        inline bool HasCinemaRequirement() const {
            auto iter = std::find(requirements.begin(), requirements.end(), u"Cinema");
            return iter != requirements.end();
        }

        inline bool HasCinema() const {
            return HasCinemaSuggestion() || HasCinemaRequirement();
        }
    };

    struct ExtraSongData {

        std::vector<DifficultyData> difficulties;

        inline bool HasCinemaSuggestionInAnyDifficulty() const {
            return std::ranges::any_of(difficulties.begin(), difficulties.end(),
            [](const auto& diff) { return diff.HasCinemaSuggestion(); });
        }

        inline bool HasCinemaRequirementInAnyDifficulty() const {
            return std::ranges::any_of(difficulties.begin(), difficulties.end(),
            [](const auto& diff) { return diff.HasCinemaRequirement(); });
        }

        inline bool HasCinemaInAnyDifficulty() const {
            return HasCinemaSuggestionInAnyDifficulty() || HasCinemaRequirementInAnyDifficulty();
        }
    };
}