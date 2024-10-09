#include "Hooks/LevelDataHooks.hpp"
#include "main.hpp"

#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MissionLevelScenesTransitionSetupDataSO.hpp"

using namespace GlobalNamespace;
using namespace Cinema::LevelData;
using namespace Cinema::Gameplay;

GlobalNamespace::GameplayCoreSceneSetupData* currentLevelData;

MAKE_AUTO_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init_1, static_cast<void (StandardLevelScenesTransitionSetupDataSO::*)(StringW, ::GlobalNamespace::IBeatmapLevelData*, ByRef<::GlobalNamespace::BeatmapKey>, GlobalNamespace::BeatmapLevel*, ::GlobalNamespace::OverrideEnvironmentSettings*, GlobalNamespace::ColorScheme*, ::GlobalNamespace::ColorScheme*, GlobalNamespace::GameplayModifiers*, ::GlobalNamespace::PlayerSpecificSettings*, GlobalNamespace::PracticeSettings*, ::GlobalNamespace::EnvironmentsListModel*, GlobalNamespace::AudioClipAsyncLoader*, BeatSaber::PerformancePresets::PerformancePreset*, ::GlobalNamespace::BeatmapDataLoader*, StringW, bool, bool, System::Nullable_1<::GlobalNamespace::RecordingToolManager::SetupData>)>(&StandardLevelScenesTransitionSetupDataSO::Init),
                        void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, ::GlobalNamespace::IBeatmapLevelData* beatmapLevelData, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, GlobalNamespace::BeatmapLevel* beatmapLevel, ::GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings, GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::ColorScheme* beatmapOverrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::PracticeSettings* practiceSettings, ::GlobalNamespace::EnvironmentsListModel* environmentsListModel, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, BeatSaber::PerformancePresets::PerformancePreset* performancePreset, ::GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, StringW backButtonText, bool useTestNoteCutSoundEffects, bool startPaused, System::Nullable_1<::GlobalNamespace::__RecordingToolManager__SetupData> recordingToolData)
{

    StandardLevelScenesTransitionSetupDataSO_Init_1(self, gameMode, beatmapLevelData, beatmapKey, beatmapLevel, overrideEnvironmentSettings, overrideColorScheme, beatmapOverrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, environmentsListModel, audioClipAsyncLoader, performancePreset, beatmapDataLoader, backButtonText, useTestNoteCutSoundEffects, startPaused, recordingToolData);
    
    levelData.gameplayCoreSceneSetupData = self->get_gameplayCoreSceneSetupData();
    levelData.mode = Mode::Standard;
    levelData.isSet = true;
}

MAKE_AUTO_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init_2, static_cast<void (StandardLevelScenesTransitionSetupDataSO::*)(StringW, ByRef<::GlobalNamespace::BeatmapKey>, GlobalNamespace::BeatmapLevel*, ::GlobalNamespace::OverrideEnvironmentSettings*, GlobalNamespace::ColorScheme*, ::GlobalNamespace::ColorScheme*, GlobalNamespace::GameplayModifiers*, ::GlobalNamespace::PlayerSpecificSettings*, GlobalNamespace::PracticeSettings*, ::GlobalNamespace::EnvironmentsListModel*, GlobalNamespace::AudioClipAsyncLoader*, ::GlobalNamespace::BeatmapDataLoader*, BeatSaber::PerformancePresets::PerformancePreset*, StringW, GlobalNamespace::BeatmapLevelsModel*, bool, bool, System::Nullable_1<::GlobalNamespace::RecordingToolManager::SetupData>)>(&StandardLevelScenesTransitionSetupDataSO::Init),
                        void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, GlobalNamespace::BeatmapLevel* beatmapLevel, ::GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings, GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::ColorScheme* beatmapOverrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::PracticeSettings* practiceSettings, ::GlobalNamespace::EnvironmentsListModel* environmentsListModel, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, ::GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, BeatSaber::PerformancePresets::PerformancePreset* performancePreset, StringW backButtonText, GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel, bool useTestNoteCutSoundEffects, bool startPaused, System::Nullable_1<::GlobalNamespace::__RecordingToolManager__SetupData> recordingToolData)
{

    StandardLevelScenesTransitionSetupDataSO_Init_2(self, gameMode, beatmapKey, beatmapLevel, overrideEnvironmentSettings, overrideColorScheme, beatmapOverrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, environmentsListModel, audioClipAsyncLoader, beatmapDataLoader, performancePreset, backButtonText, beatmapLevelsModel, useTestNoteCutSoundEffects, startPaused, recordingToolData);
    
    levelData.gameplayCoreSceneSetupData = self->get_gameplayCoreSceneSetupData();
    levelData.mode = Mode::Standard;
    levelData.isSet = true;
}

MAKE_AUTO_HOOK_MATCH(MultiplayerLevelScenesTransitionSetupDataSO_Init, &MultiplayerLevelScenesTransitionSetupDataSO::Init, void, MultiplayerLevelScenesTransitionSetupDataSO* self, StringW gameMode, ByRef<GlobalNamespace::BeatmapKey> beatmapKey, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::IBeatmapLevelData* beatmapLevelData, GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::PracticeSettings* practiceSettings, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, BeatSaber::PerformancePresets::PerformancePreset* performancePreset, GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, bool useTestNoteCutSoundEffects)
{
    MultiplayerLevelScenesTransitionSetupDataSO_Init(self, gameMode, beatmapKey, beatmapLevel, beatmapLevelData, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, audioClipAsyncLoader, performancePreset, beatmapDataLoader, useTestNoteCutSoundEffects);
    levelData.gameplayCoreSceneSetupData = self->get_gameplayCoreSceneSetupData();
    levelData.mode = Mode::Multiplayer;
    levelData.isSet = true;
}

MAKE_AUTO_HOOK_MATCH(MissionLevelScenesTransitionSetupDataSO_Init, static_cast<void(MissionLevelScenesTransitionSetupDataSO::*)(StringW, GlobalNamespace::IBeatmapLevelData*, ByRef<::GlobalNamespace::BeatmapKey> , GlobalNamespace::BeatmapLevel*, ArrayW<::GlobalNamespace::MissionObjective*, Array<::GlobalNamespace::MissionObjective*>*>, GlobalNamespace::ColorScheme*, GlobalNamespace::GameplayModifiers*, GlobalNamespace::PlayerSpecificSettings*, GlobalNamespace::EnvironmentsListModel*, GlobalNamespace::AudioClipAsyncLoader*, BeatSaber::PerformancePresets::PerformancePreset*, GlobalNamespace::BeatmapDataLoader*, StringW)>(&MissionLevelScenesTransitionSetupDataSO::Init), 
                        void, MissionLevelScenesTransitionSetupDataSO* self, StringW missionId, GlobalNamespace::IBeatmapLevelData* beatmapLevelData, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, GlobalNamespace::BeatmapLevel* beatmapLevel, ArrayW<::GlobalNamespace::MissionObjective*, Array<::GlobalNamespace::MissionObjective*>*> missionObjectives, GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::EnvironmentsListModel* environmentsListModel, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, BeatSaber::PerformancePresets::PerformancePreset* performancePreset, GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, StringW backButtonText)
{
    MissionLevelScenesTransitionSetupDataSO_Init(self, missionId, beatmapLevelData, beatmapKey, beatmapLevel, missionObjectives, overrideColorScheme, gameplayModifiers, playerSpecificSettings, environmentsListModel, audioClipAsyncLoader, performancePreset, beatmapDataLoader, backButtonText);
    levelData.gameplayCoreSceneSetupData = self->get_gameplayCoreSceneSetupData();
    levelData.mode = Mode::Mission;
    levelData.isSet = true;
}

namespace Cinema::LevelData
{
    Gameplay::LevelData levelData;
}