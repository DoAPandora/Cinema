#pragma once
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"

namespace Cinema::LevelData {
    void OnMenuLoaded();
    GlobalNamespace::GameplayCoreSceneSetupData* get_currentGameplayCoreData(); 
}