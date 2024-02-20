#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "VideoMenu/VideoMenu.hpp"
#include "Video/VideoLoader.hpp"
#include "Util/Util.hpp"

#include "bsml/shared/BSML.hpp"

#include "Screen/PlaybackController.hpp"

#include "assets.hpp"
#include "pinkcore/shared/RequirementAPI.hpp"

#include "GlobalNamespace/DefaultScenesTransitionsFromInit.hpp"

#include "pythonlib/shared/Utils/FileUtils.hpp"

#include "bs-events/shared/BSEvents.hpp"

#include "Screen/CurvedSurface.hpp"

#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/MeshRenderer.hpp"

modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

MAKE_HOOK_MATCH(DefaultScenesTransitionsFromInit_TransitionToNextScene, &GlobalNamespace::DefaultScenesTransitionsFromInit::TransitionToNextScene, void, GlobalNamespace::DefaultScenesTransitionsFromInit* self, bool goStraightToMenu, bool goStraightToEditor, bool goToRecordingToolScene)
{
    DefaultScenesTransitionsFromInit_TransitionToNextScene(self, goStraightToMenu, goStraightToEditor, goToRecordingToolScene);
    Cinema::PlaybackController::Create();
    Cinema::VideoMenu::get_instance()->AddTab();
    Cinema::VideoMenu::get_instance()->Init();
}

void TestCurvedSurface(GlobalNamespace::ScenesTransitionSetupDataSO*)
{
    DEBUG("Testing curved surface");
    auto go = UnityEngine::GameObject::New_ctor("CurvedSurfaceTest");
    UnityEngine::Object::DontDestroyOnLoad(go);
    go->AddComponent<UnityEngine::MeshFilter*>();
    go->get_transform()->set_position({0, 1, 1});
    go->AddComponent<UnityEngine::MeshRenderer*>();
    auto body = go->AddComponent<Cinema::CurvedSurface*>();

    body->Initialise(16.0f / 10.0f, 9.0f / 10.0f, 5, 60, 30, false);
    body->Generate();
}

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, {false, true});
    return *logger;
}

CINEMA_EXPORT void setup(CModInfo* info) noexcept {
    *info = modInfo.to_c();

    INFO("Beginning setup");
	
    getModConfig().Init(modInfo);
    Cinema::VideoLoader::Init();

    INFO("Completed setup!");
}

CINEMA_EXPORT void late_load() noexcept {
    il2cpp_functions::Init();
    BSEvents::Init();
	custom_types::Register::AutoRegister();

    mkpath(VIDEO_DIR);
    mkpath(THUMBNAIL_DIR);

    INFO("Installing hooks...");
    
    Hooks::InstallHooks(getLogger());
    Cinema::Hooks::InstallLevelDataHook();
    INSTALL_HOOK(getLogger(), DefaultScenesTransitionsFromInit_TransitionToNextScene);

    INFO("Installed all hooks!");

    std::string ytdlp = FileUtils::getScriptsPath() + "/yt_dlp";
    if(!direxists(ytdlp))
        FileUtils::ExtractZip(IncludedAssets::ytdlp_zip, ytdlp);

    // BSML::Init();
//    BSML::Register::RegisterGameplaySetupTab("Cinema", MOD_ID "_settings", Cinema::VideoMenuManager::get_instance(), BSML::MenuType::Solo);
//    BSML::Register::RegisterGameplaySetupTab<Cinema::VideoMenu*>("Cinema");

    PinkCore::RequirementAPI::RegisterInstalled("Cinema");

//    BSEvents::lateMenuSceneLoadedFresh += TestCurvedSurface;
}

#include "bsml/shared/BSMLDataCache.hpp"

BSML_DATACACHE(settings)
{
    return IncludedAssets::VideoMenu_bsml;
}