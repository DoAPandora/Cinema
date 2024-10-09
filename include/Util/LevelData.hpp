#pragma once

namespace GlobalNamespace
{
    class GameplayCoreSceneSetupData;
}

namespace Cinema::Gameplay
{
    enum class Mode
    {
        None,
        Standard,
        Multiplayer,
        Mission
    };

    struct LevelData
    {
    
        GlobalNamespace::GameplayCoreSceneSetupData* gameplayCoreSceneSetupData;
        Mode mode = Mode::None;
        bool isSet;

        void Clear()
        {
            isSet = false;
            gameplayCoreSceneSetupData = nullptr;
            mode = Mode::None;
        }
    };
}