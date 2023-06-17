#include "main.hpp"
#include "Hooks/LevelData.hpp"

#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"

using namespace GlobalNamespace;

GlobalNamespace::GameplayCoreSceneSetupData* currentLevelData;

MAKE_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, &StandardLevelScenesTransitionSetupDataSO::Init, void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, IDifficultyBeatmap* difficultyBeatmap, IPreviewBeatmapLevel* previewBeatmapLevel, OverrideEnvironmentSettings* overrideEnvironmentSettings, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, StringW backButtonText, bool startPaused, bool useTestNoteCutSoundEffects, BeatmapDataCache* beatmapDataCache)
{
    StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, startPaused, useTestNoteCutSoundEffects, beatmapDataCache);
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

void Cinema::Hooks::InstallLevelDataHook()
{
    INSTALL_HOOK(getLogger(), StandardLevelScenesTransitionSetupDataSO_Init);
}