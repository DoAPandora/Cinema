#include "main.hpp"
#include "Video/VideoLoader.hpp"
#include "VideoMenu/VideoMenu.hpp"

#include "bsml/shared/BSML.hpp"

#include "Screen/PlaybackController.hpp"

#include "assets.hpp"

#include "GlobalNamespace/DefaultScenesTransitionsFromInit.hpp"

#include "songcore/shared/SongCore.hpp"

#include "pythonlib/shared/Utils/FileUtils.hpp"

#include "bs-events/shared/BSEvents.hpp"

modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

Cinema::Placement Cinema::Placement::MenuPlacement{UnityEngine::Vector3(0, 4, 16), UnityEngine::Vector3(0, 0, 0), 8.0f};

MAKE_AUTO_HOOK_MATCH(DefaultScenesTransitionsFromInit_TransitionToNextScene, &GlobalNamespace::DefaultScenesTransitionsFromInit::TransitionToNextScene, void, GlobalNamespace::DefaultScenesTransitionsFromInit* self, bool goStraightToMenu, bool goStraightToEditor, bool goToRecordingToolScene)
{
    DefaultScenesTransitionsFromInit_TransitionToNextScene(self, goStraightToMenu, goStraightToEditor, goToRecordingToolScene);
    DEBUG("Creating PlaybackController");
    Cinema::PlaybackController::Create();
    Cinema::VideoMenu::get_instance()->Init();
    Cinema::VideoMenu::get_instance()->AddTab();
}

// void TestCurvedSurface(GlobalNamespace::ScenesTransitionSetupDataSO *)
// {
//     DEBUG("Testing curved surface");
//     auto go = UnityEngine::GameObject::New_ctor("CurvedSurfaceTest");
//     UnityEngine::Object::DontDestroyOnLoad(go);
//     go->get_transform()->set_position({0, 1, 1});
//     auto body = go->AddComponent<Cinema::CurvedSurface *>();

//     body->Initialise(16.0f / 10.0f, 9.0f / 10.0f, 5, 60, 30, false);
//     body->Generate();
// }

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

    std::string ytdlp = FileUtils::getScriptsPath() + "/yt_dlp";
    if(!direxists(ytdlp))
        FileUtils::ExtractZip(IncludedAssets::ytdlp_zip, ytdlp);

    BSML::Init();

    SongCore::API::Capabilities::RegisterCapability("Cinema");
}

#include "bsml/shared/BSMLDataCache.hpp"

BSML_DATACACHE(settings)
{
    return IncludedAssets::VideoMenu_bsml;
}