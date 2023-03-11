#include "main.hpp"
#include "hooks.hpp"
#include "VideoPlayer.hpp"
#include "WIPUI/VideoMenuManager.hpp"

#include "GlobalNamespace/GamePause.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"

Cinema::VideoPlayer* videoPlayer = nullptr;

MAKE_HOOK_MATCH(AudioTimeSyncController_Start, &GlobalNamespace::AudioTimeSyncController::Start, void, GlobalNamespace::AudioTimeSyncController* self)
{
    AudioTimeSyncController_Start(self);
    auto manager = Cinema::VideoMenuManager::get_instance();
    if(manager->ShouldCreateScreen())
    {
        getLogger().info("Creating screen");
        videoPlayer = Cinema::VideoPlayer::CreateVideoPlayer(self->audioSource);
        getLogger().info("Setting video path to %s", manager->GetCurrentVideoPath().c_str());
        videoPlayer->set_url(manager->GetCurrentVideoPath());
        float offset = manager->get_currentLevelData().offset;
        offset /= 1000;
        videoPlayer->Start(offset + 0.5f);
    }
    else
        videoPlayer = nullptr;
}

MAKE_HOOK_MATCH(GamePause_Resume, &GlobalNamespace::GamePause::Resume, void, GlobalNamespace::GamePause* self)
{
    getLogger().info("GamePause_Resume");
    GamePause_Resume(self);
    if(videoPlayer)
    {
        getLogger().info("VideoPlayer exists, resuming");
        videoPlayer->Play();
    }
}

MAKE_HOOK_MATCH(GamePause_Pause, &GlobalNamespace::GamePause::Pause, void, GlobalNamespace::GamePause* self) {
    getLogger().info("GamePause_Pause");
    GamePause_Pause(self);
    if(videoPlayer)
    {
        getLogger().info("VideoPlayer exists, pausing");
        videoPlayer->Pause();
    }
}

void Cinema::Hooks::InstallVideoPlayerHooks() {
    INSTALL_HOOK(getLogger(), AudioTimeSyncController_Start);
    INSTALL_HOOK(getLogger(), GamePause_Resume);
    INSTALL_HOOK(getLogger(), GamePause_Pause);
}