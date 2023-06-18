#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Screen/PlaybackController.hpp"

#include "custom-types/shared/delegate.hpp"

#include "UnityEngine/SceneManagement/SceneManager.hpp"
#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/Resources.hpp"

#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/GamePause.hpp"
#include "GlobalNamespace/GameScenesManager.hpp"
#include "GlobalNamespace/RichPresenceManager.hpp"

#include "Zenject/DiContainer.hpp"

#include "System/Action_2.hpp"

using namespace GlobalNamespace;

bool lastMainSceneWasNotMenu = false;
bool runGameSceneLoaded = false, runMenuSceneLoaded = false, runMenuSceneLoadedFresh = false;

// Do stuff safely after scene transition finished (Liveness) - No need to create and add delegate
MAKE_HOOK_MATCH(RichPresenceManager_HandleGameScenesManagerTransitionDidFinish, &RichPresenceManager::HandleGameScenesManagerTransitionDidFinish, void, RichPresenceManager* self, ScenesTransitionSetupDataSO* transitionSetupData, Zenject::DiContainer* container)
{
    RichPresenceManager_HandleGameScenesManagerTransitionDidFinish(self, transitionSetupData, container);

    // Cinema::PlaybackController::get_instance();

    if(runGameSceneLoaded)
    {
        Cinema::PlaybackController::get_instance()->GameSceneLoaded();
        runGameSceneLoaded = false;
    }
    if(runMenuSceneLoaded)
    {
        Cinema::LevelData::OnMenuLoaded();
        Cinema::PlaybackController::get_instance()->OnMenuSceneLoaded();
        runMenuSceneLoaded = false;
    }
    if(runMenuSceneLoadedFresh)
    {
        Cinema::LevelData::OnMenuLoaded();
        Cinema::PlaybackController::get_instance()->OnMenuSceneLoadedFresh(transitionSetupData);
        runMenuSceneLoadedFresh = false;
    }
}

MAKE_HOOK_MATCH(SceneManager_Internal_ActiveSceneChanged, &UnityEngine::SceneManagement::SceneManager::Internal_ActiveSceneChanged, void, UnityEngine::SceneManagement::Scene previousActiveScene, UnityEngine::SceneManagement::Scene newActiveScene)
{
    SceneManager_Internal_ActiveSceneChanged(previousActiveScene, newActiveScene);
    if(!previousActiveScene.IsValid() || !newActiveScene.IsValid())
        return getLogger().info("Scene(s) were null");

    if(newActiveScene.get_name() == "GameCore")
    {
        Cinema::PlaybackController::get_instance()->GameSceneActive();
        runGameSceneLoaded = true;
    }
    else if(newActiveScene.get_name() == "MainMenu")
    {
        if(previousActiveScene.get_name() == "EmptyTransition" && !lastMainSceneWasNotMenu)
            runMenuSceneLoadedFresh = true;
        else
            runMenuSceneLoaded = true;

        lastMainSceneWasNotMenu = false;
    }
    if(newActiveScene.get_name() == "GameCore" || newActiveScene.get_name() == "Credits")
        lastMainSceneWasNotMenu = true;
}

MAKE_HOOK_MATCH(VideoPlayer_InvokePrepareCompletedCallback_Internal, &UnityEngine::Video::VideoPlayer::InvokePrepareCompletedCallback_Internal, void, UnityEngine::Video::VideoPlayer* source)
{
    VideoPlayer_InvokePrepareCompletedCallback_Internal(source);
    if(il2cpp_utils::try_cast<Cinema::VideoPlayer>(source))
        Cinema::PlaybackController::get_instance()->OnPrepareComplete();
}

MAKE_HOOK_MATCH(VideoPlayer_InvokeFrameReadyCallback_Internal, &UnityEngine::Video::VideoPlayer::InvokeFrameReadyCallback_Internal, void, UnityEngine::Video::VideoPlayer* source, long frame)
{
    VideoPlayer_InvokeFrameReadyCallback_Internal(source, frame);
    if(il2cpp_utils::try_cast<Cinema::VideoPlayer>(source))
        Cinema::PlaybackController::get_instance()->FrameReady(frame);
}

MAKE_HOOK_MATCH(GamePause_Resume, &GamePause::Resume, void, GamePause* self)
{
    GamePause_Resume(self);
    Cinema::PlaybackController::get_instance()->ResumeVideo();
}

MAKE_HOOK_MATCH(GamePause_Pause, &GamePause::Pause, void, GamePause* self)
{
    GamePause_Pause(self);
    Cinema::PlaybackController::get_instance()->PauseVideo();
}

void Cinema::Hooks::InstallPlaybackControllerEventHooks() {
    INSTALL_HOOK(getLogger(), RichPresenceManager_HandleGameScenesManagerTransitionDidFinish);
    INSTALL_HOOK(getLogger(), SceneManager_Internal_ActiveSceneChanged);
    INSTALL_HOOK(getLogger(), GamePause_Resume);
    INSTALL_HOOK(getLogger(), GamePause_Pause);
    INSTALL_HOOK(getLogger(), VideoPlayer_InvokePrepareCompletedCallback_Internal);
    INSTALL_HOOK(getLogger(), VideoPlayer_InvokeFrameReadyCallback_Internal);
}