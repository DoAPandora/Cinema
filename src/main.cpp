#include "main.hpp"
#include "Download/DownloadController.hpp"
#include "Video/VideoLoader.hpp"
#include "VideoMenu/VideoMenu.hpp"
#include "Hooks/LevelDataHooks.hpp"
#include "Screen/PlaybackController.hpp"
#include "Settings/SettingsViewController.hpp"

#include "bsml/shared/BSML.hpp"

#include "assets.hpp"

#include "GlobalNamespace/DefaultScenesTransitionsFromInit.hpp"

#include "songcore/shared/SongCore.hpp"

#include "bs-events/shared/BSEvents.hpp"

#include "UnityEngine/SceneManagement/SceneManagement.hpp"

#include "custom-types/shared/delegate.hpp"

modloader::ModInfo modInfo{MOD_ID, VERSION, 0};



MAKE_AUTO_HOOK_MATCH(DefaultScenesTransitionsFromInit_TransitionToNextScene, &GlobalNamespace::DefaultScenesTransitionsFromInit::TransitionToNextScene, void, GlobalNamespace::DefaultScenesTransitionsFromInit* self, bool goStraightToMenu, bool goStraightToEditor, bool goToRecordingToolScene)
{
    DefaultScenesTransitionsFromInit_TransitionToNextScene(self, goStraightToMenu, goStraightToEditor, goToRecordingToolScene);
    DEBUG("Creating PlaybackController");
    Cinema::PlaybackController::Create();
    Cinema::VideoMenu::get_instance()->Init();
    Cinema::VideoMenu::get_instance()->AddTab();
}


CINEMA_EXPORT void setup(CModInfo* info) noexcept
{
    INFO("Beginning setup");

    *info = modInfo.to_c();

    Paper::Logger::RegisterFileContextId(Logger.tag);

    getModConfig().Init(modInfo);
    Cinema::VideoLoader::Init();

    INFO("Completed setup!");
}

CINEMA_EXPORT void late_load() noexcept
{
    il2cpp_functions::Init();
    custom_types::Register::AutoRegister();
    BSEvents::Init();

    mkpath(VIDEO_DIR);
    mkpath(THUMBNAIL_DIR);

    INFO("Installing hooks...");

    Hooks::InstallHooks(Logger);

    INFO("Installed all hooks!");

    Cinema::DownloadController::Setup();

    BSML::Init();
    BSML::Register::RegisterMainMenu<Cinema::SettingsViewController*>("Cinema", "Cinema", "Cinema Settings");

    SongCore::API::Capabilities::RegisterCapability("Cinema");

    using namespace UnityEngine::SceneManagement;
    auto activeSceneChanged = SceneManager::getStaticF_activeSceneChanged();
    auto callback = custom_types::MakeDelegate<decltype(activeSceneChanged)>(std::function([](Scene prev, Scene next){
        if(next.get_name() == "MainMenu")
        {
            Cinema::LevelData::levelData.Clear();
        }
    }));
    activeSceneChanged = (decltype(activeSceneChanged))System::Delegate::Combine(activeSceneChanged, callback);
    SceneManager::setStaticF_activeSceneChanged(activeSceneChanged);
}

#include "bsml/shared/BSMLDataCache.hpp"

BSML_DATACACHE(settings)
{
    return IncludedAssets::VideoMenu_bsml;
}