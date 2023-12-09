#include "main.hpp"
#include "Hooks/Hooks.hpp"

#include "BSML/shared/BSML.hpp"

#include "Screen/PlaybackController.hpp"

#include "UI/VideoMenuManager.hpp"
#include "assets.hpp"
#include "pinkcore/shared/RequirementAPI.hpp"

#include "GlobalNamespace/DefaultScenesTransitionsFromInit.hpp"

#include "pythonlib/shared/Utils/FileUtils.hpp"

#include "bs-events/shared/BSEvents.hpp"

#include "Screen/CurvedSurface.hpp"

#include "UnityEngine/MeshFilter.hpp"
#include "UnityEngine/MeshRenderer.hpp"

ModInfo modInfo;

MAKE_HOOK_MATCH(DefaultScenesTransitionsFromInit_TransitionToNextScene, &GlobalNamespace::DefaultScenesTransitionsFromInit::TransitionToNextScene, void, GlobalNamespace::DefaultScenesTransitionsFromInit* self, bool goStraightToMenu, bool goStraightToEditor, bool goToRecordingToolScene)
{
    DefaultScenesTransitionsFromInit_TransitionToNextScene(self, goStraightToMenu, goStraightToEditor, goToRecordingToolScene);
    Cinema::PlaybackController::Create();
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
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	
    getModConfig().Init(modInfo);
    INFO("Completed setup!");
}

extern "C" void load() {
    il2cpp_functions::Init();
    BSEvents::Init();

    mkpath(VIDEO_DIR);
    mkpath(THUMBNAIL_DIR);

    INFO("Installing hooks...");
    Hooks::InstallHooks(getLogger());
    Cinema::Hooks::InstallVideoDownloadHooks();
    Cinema::Hooks::InstallLevelDataHook();
    INSTALL_HOOK(getLogger(), DefaultScenesTransitionsFromInit_TransitionToNextScene);
    INFO("Installed all hooks!");

    std::string ytdlp = FileUtils::getScriptsPath() + "/yt_dlp";
    if(!direxists(ytdlp))
        FileUtils::ExtractZip(IncludedAssets::ytdlp_zip, ytdlp);

    BSML::Init();
	custom_types::Register::AutoRegister();
    BSML::Register::RegisterGameplaySetupTab("Cinema", MOD_ID "_settings", Cinema::VideoMenuManager::get_instance(), BSML::MenuType::Solo);

    PinkCore::RequirementAPI::RegisterInstalled("Cinema");

//    BSEvents::lateMenuSceneLoadedFresh += TestCurvedSurface;
}

#include "bsml/shared/BSMLDataCache.hpp"

BSML_DATACACHE(settings)
{
    return IncludedAssets::VideoMenu_bsml;
}