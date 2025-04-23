#include "Hooks/LevelDataHooks.hpp"
#include "main.hpp"

#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MissionLevelScenesTransitionSetupDataSO.hpp"

using namespace GlobalNamespace;
using namespace Cinema::LevelData;
using namespace Cinema::Gameplay;

GlobalNamespace::GameplayCoreSceneSetupData* currentLevelData;

MAKE_AUTO_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init_1, static_cast<void (StandardLevelScenesTransitionSetupDataSO::*)(StringW gameMode, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings, GlobalNamespace::ColorScheme* playerOverrideColorScheme, bool playerOverrideLightshowColors, GlobalNamespace::ColorScheme* beatmapOverrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::PracticeSettings* practiceSettings, GlobalNamespace::EnvironmentsListModel* environmentsListModel, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, GlobalNamespace::SettingsManager* settingsManager, StringW backButtonText, GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel, GlobalNamespace::BeatmapLevelsEntitlementModel* beatmapLevelsEntitlementModel, bool useTestNoteCutSoundEffects, bool startPaused, System::Nullable_1<::GlobalNamespace::RecordingToolManager_SetupData> recordingToolData)>(&StandardLevelScenesTransitionSetupDataSO::Init), 
    void, StandardLevelScenesTransitionSetupDataSO* self,  StringW gameMode, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings, GlobalNamespace::ColorScheme* playerOverrideColorScheme, bool playerOverrideLightshowColors, GlobalNamespace::ColorScheme* beatmapOverrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::PracticeSettings* practiceSettings, GlobalNamespace::EnvironmentsListModel* environmentsListModel, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, GlobalNamespace::SettingsManager* settingsManager, StringW backButtonText, GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel, GlobalNamespace::BeatmapLevelsEntitlementModel* beatmapLevelsEntitlementModel, bool useTestNoteCutSoundEffects, bool startPaused, System::Nullable_1<::GlobalNamespace::RecordingToolManager_SetupData> recordingToolData)
{

    StandardLevelScenesTransitionSetupDataSO_Init_1(self, gameMode, beatmapKey, beatmapLevel, overrideEnvironmentSettings, playerOverrideColorScheme, playerOverrideLightshowColors, beatmapOverrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, environmentsListModel, audioClipAsyncLoader, beatmapDataLoader, settingsManager, backButtonText, beatmapLevelsModel, beatmapLevelsEntitlementModel, useTestNoteCutSoundEffects, startPaused, recordingToolData);
    
    levelData.gameplayCoreSceneSetupData = self->get_gameplayCoreSceneSetupData();
    levelData.mode = Mode::Standard;
    levelData.isSet = true;
}

MAKE_AUTO_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init_2, static_cast<void (StandardLevelScenesTransitionSetupDataSO::*)(StringW , GlobalNamespace::IBeatmapLevelData*, ByRef<::GlobalNamespace::BeatmapKey>, GlobalNamespace::BeatmapLevel*, GlobalNamespace::OverrideEnvironmentSettings* , GlobalNamespace::ColorScheme*, bool, GlobalNamespace::ColorScheme*, GlobalNamespace::GameplayModifiers*, GlobalNamespace::PlayerSpecificSettings*, GlobalNamespace::PracticeSettings*, GlobalNamespace::EnvironmentsListModel*, GlobalNamespace::AudioClipAsyncLoader*, GlobalNamespace::SettingsManager*, GlobalNamespace::BeatmapDataLoader*, GlobalNamespace::BeatmapLevelsEntitlementModel*, StringW, bool, bool, System::Nullable_1<::GlobalNamespace::RecordingToolManager_SetupData>)>(&StandardLevelScenesTransitionSetupDataSO::Init),
    void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, ::GlobalNamespace::IBeatmapLevelData* beatmapLevelData, ::ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, ::GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings, GlobalNamespace::ColorScheme* playerOverrideColorScheme, bool playerOverrideLightshowColors, GlobalNamespace::ColorScheme* beatmapOverrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::PracticeSettings* practiceSettings, GlobalNamespace::EnvironmentsListModel* environmentsListModel, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, GlobalNamespace::SettingsManager* settingsManager, GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, GlobalNamespace::BeatmapLevelsEntitlementModel* beatmapLevelsEntitlementModel, StringW backButtonText, bool useTestNoteCutSoundEffects, bool startPaused, System::Nullable_1<::GlobalNamespace::RecordingToolManager_SetupData> recordingToolData)
{

    StandardLevelScenesTransitionSetupDataSO_Init_2(self, gameMode, beatmapLevelData, beatmapKey, beatmapLevel, overrideEnvironmentSettings, playerOverrideColorScheme, playerOverrideLightshowColors, beatmapOverrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, environmentsListModel, audioClipAsyncLoader, settingsManager, beatmapDataLoader, beatmapLevelsEntitlementModel, backButtonText, useTestNoteCutSoundEffects, startPaused, recordingToolData);
    
    levelData.gameplayCoreSceneSetupData = self->get_gameplayCoreSceneSetupData();
    levelData.mode = Mode::Standard;
    levelData.isSet = true;
}

MAKE_AUTO_HOOK_MATCH(MultiplayerLevelScenesTransitionSetupDataSO_Init, &MultiplayerLevelScenesTransitionSetupDataSO::Init, void, MultiplayerLevelScenesTransitionSetupDataSO* self, StringW gameMode, ByRef<GlobalNamespace::BeatmapKey> beatmapKey, GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::IBeatmapLevelData* beatmapLevelData, GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::PracticeSettings* practiceSettings, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, GlobalNamespace::SettingsManager* settingsManager, GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, bool useTestNoteCutSoundEffects)
{
    MultiplayerLevelScenesTransitionSetupDataSO_Init(self, gameMode, beatmapKey, beatmapLevel, beatmapLevelData, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, audioClipAsyncLoader, settingsManager, beatmapDataLoader, useTestNoteCutSoundEffects);
    levelData.gameplayCoreSceneSetupData = self->get_gameplayCoreSceneSetupData();
    levelData.mode = Mode::Multiplayer;
    levelData.isSet = true;
}

MAKE_AUTO_HOOK_MATCH(MissionLevelScenesTransitionSetupDataSO_Init_1, static_cast<void(MissionLevelScenesTransitionSetupDataSO::*)( StringW, ByRef<GlobalNamespace::BeatmapKey>, GlobalNamespace::BeatmapLevel*, ArrayW<GlobalNamespace::MissionObjective*>, GlobalNamespace::ColorScheme*, GlobalNamespace::GameplayModifiers*, GlobalNamespace::PlayerSpecificSettings*, GlobalNamespace::EnvironmentsListModel*, GlobalNamespace::BeatmapLevelsModel*, GlobalNamespace::AudioClipAsyncLoader*, GlobalNamespace::SettingsManager*, GlobalNamespace::BeatmapDataLoader*, StringW)>(&MissionLevelScenesTransitionSetupDataSO::Init),
    void, MissionLevelScenesTransitionSetupDataSO* self, StringW missionId, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, GlobalNamespace::BeatmapLevel* beatmapLevel, ArrayW<::GlobalNamespace::MissionObjective*> missionObjectives, GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::EnvironmentsListModel* environmentsListModel, GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, GlobalNamespace::SettingsManager* settingsManager, GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, StringW backButtonText)
{
    MissionLevelScenesTransitionSetupDataSO_Init_1(self, missionId, beatmapKey, beatmapLevel, missionObjectives, overrideColorScheme, gameplayModifiers, playerSpecificSettings, environmentsListModel, beatmapLevelsModel, audioClipAsyncLoader, settingsManager, beatmapDataLoader, backButtonText);
    levelData.gameplayCoreSceneSetupData = self->get_gameplayCoreSceneSetupData();
    levelData.mode = Mode::Mission;
    levelData.isSet = true;
}

MAKE_AUTO_HOOK_MATCH(MissionLevelScenesTransitionSetupDataSO_Init_2, static_cast<void(MissionLevelScenesTransitionSetupDataSO::*)( StringW, GlobalNamespace::IBeatmapLevelData*, ByRef<GlobalNamespace::BeatmapKey>, GlobalNamespace::BeatmapLevel*, ArrayW<GlobalNamespace::MissionObjective*>, GlobalNamespace::ColorScheme*, GlobalNamespace::GameplayModifiers*, GlobalNamespace::PlayerSpecificSettings*, GlobalNamespace::EnvironmentsListModel*, GlobalNamespace::AudioClipAsyncLoader*, GlobalNamespace::SettingsManager*, GlobalNamespace::BeatmapDataLoader*, StringW)>(&MissionLevelScenesTransitionSetupDataSO::Init),
    void, MissionLevelScenesTransitionSetupDataSO* self, StringW missionId, GlobalNamespace::IBeatmapLevelData* beatmapLevelData, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, GlobalNamespace::BeatmapLevel* beatmapLevel, ArrayW<::GlobalNamespace::MissionObjective*> missionObjectives, GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::EnvironmentsListModel* environmentsListModel, GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, GlobalNamespace::SettingsManager* settingsManager, GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, StringW backButtonText)
{
    MissionLevelScenesTransitionSetupDataSO_Init_2(self, missionId, beatmapLevelData, beatmapKey, beatmapLevel, missionObjectives, overrideColorScheme, gameplayModifiers, playerSpecificSettings, environmentsListModel, audioClipAsyncLoader, settingsManager, beatmapDataLoader, backButtonText);
    levelData.gameplayCoreSceneSetupData = self->get_gameplayCoreSceneSetupData();
    levelData.mode = Mode::Mission;
    levelData.isSet = true;
}

namespace Cinema::LevelData
{
    Gameplay::LevelData levelData;
}