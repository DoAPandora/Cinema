#include "main.hpp"
#include "Util/Events.hpp"

namespace Cinema::Events {

    UnorderedEventCallback<bool> cinemaActivated;
    UnorderedEventCallback<GlobalNamespace::BeatmapLevel*> levelSelected;
    UnorderedEventCallback<ExtraSongDataArgs> difficultySelected;

    template <typename T>
    void InvokeSafe(const UnorderedEventCallback<T>& event, const T& arg, std::string eventName)
    {
        try {
            event.invoke(arg);
        } catch (std::exception& e) {
            ERROR("Exception thrown in {}: {}", eventName, e.what());
        }
    }

    void InvokeSceneTransitionEvents(std::optional<VideoConfig> videoConfig)
    {
        if (!getModConfig().enabled.GetValue() || videoConfig == std::nullopt)
        {
            cinemaActivated.invoke(false);
            return;
        }

        auto activated = videoConfig->get_isPlayable();
        cinemaActivated.invoke(activated);
    }

    void SetSelectedLevel(GlobalNamespace::BeatmapLevel* level)
    {
        InvokeSafe(levelSelected, level, "LevelSelected");
    }

    void SetExtraSongData(const ExtraSongData::ExtraSongData& songData, const ExtraSongData::DifficultyData& selectedDifficultyData)
    {
        InvokeSafe(difficultySelected, {songData, selectedDifficultyData}, "DifficultySelected");
    }
}