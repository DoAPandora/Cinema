#include "main.hpp"
#include "Hooks/LevelData.hpp"

#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"

using namespace GlobalNamespace;

GlobalNamespace::GameplayCoreSceneSetupData* currentLevelData;
// Casting override methods is pain
MAKE_AUTO_HOOK_FIND(StandardLevelScenesTransitionSetupDataSO_Init, classof(StandardLevelScenesTransitionSetupDataSO*), "Init", void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, ::GlobalNamespace::BeatmapLevel* beatmapLevel, GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings, ::GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::ColorScheme* beatmapOverrideColorScheme, ::GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, ::GlobalNamespace::PracticeSettings* practiceSettings, GlobalNamespace::EnvironmentsListModel* environmentsListModel, ::GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader, GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, ::StringW backButtonText, ::GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel, bool useTestNoteCutSoundEffects, bool startPaused, ::System::Nullable_1<GlobalNamespace::RecordingToolManager::SetupData> recordingToolData)
{
    StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, beatmapKey, beatmapLevel, overrideEnvironmentSettings, overrideColorScheme, beatmapOverrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, environmentsListModel, audioClipAsyncLoader, beatmapDataLoader, backButtonText, beatmapLevelsModel, useTestNoteCutSoundEffects, startPaused, recordingToolData);
    currentLevelData = self->gameplayCoreSceneSetupData;
}

namespace Cinema::LevelData {

    void OnMenuLoaded()
    {
        currentLevelData = nullptr;
    }

    GlobalNamespace::GameplayCoreSceneSetupData* get_currentGameplayCoreData()
    {
        return currentLevelData;
    }
}