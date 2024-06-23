#include "Hooks/LevelData.hpp"
#include "main.hpp"

#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"

using namespace GlobalNamespace;

GlobalNamespace::GameplayCoreSceneSetupData* currentLevelData;
MAKE_AUTO_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, static_cast<void (StandardLevelScenesTransitionSetupDataSO::*)(StringW, ::GlobalNamespace::IBeatmapLevelData*, ByRef<::GlobalNamespace::BeatmapKey>, GlobalNamespace::BeatmapLevel*, ::GlobalNamespace::OverrideEnvironmentSettings*, GlobalNamespace::ColorScheme*, ::GlobalNamespace::ColorScheme*, GlobalNamespace::GameplayModifiers*, ::GlobalNamespace::PlayerSpecificSettings*, GlobalNamespace::PracticeSettings*, ::GlobalNamespace::EnvironmentsListModel*, GlobalNamespace::AudioClipAsyncLoader*, BeatSaber::PerformancePresets::PerformancePreset*, ::GlobalNamespace::BeatmapDataLoader*, StringW, bool, bool, System::Nullable_1<::GlobalNamespace::RecordingToolManager::SetupData>)>(&StandardLevelScenesTransitionSetupDataSO::Init),
                     void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, ::GlobalNamespace::IBeatmapLevelData* beatmapLevelData, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, GlobalNamespace::BeatmapLevel* beatmapLevel, ::GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings, GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::ColorScheme* beatmapOverrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::PracticeSettings* practiceSettings, ::GlobalNamespace::EnvironmentsListModel* environmentsListModel, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, BeatSaber::PerformancePresets::PerformancePreset* performancePreset, ::GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, StringW backButtonText, bool useTestNoteCutSoundEffects, bool startPaused, System::Nullable_1<::GlobalNamespace::__RecordingToolManager__SetupData> recordingToolData)
{

    StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, beatmapLevelData, beatmapKey, beatmapLevel, overrideEnvironmentSettings, overrideColorScheme, beatmapOverrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, environmentsListModel, audioClipAsyncLoader, performancePreset, beatmapDataLoader, backButtonText, useTestNoteCutSoundEffects, startPaused, recordingToolData);
    currentLevelData = self->gameplayCoreSceneSetupData;
}

namespace Cinema::LevelData
{

    void OnMenuLoaded()
    {
        currentLevelData = nullptr;
    }

    GlobalNamespace::GameplayCoreSceneSetupData* get_currentGameplayCoreData()
    {
        return currentLevelData;
    }
} // namespace Cinema::LevelData