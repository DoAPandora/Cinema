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

ModInfo modInfo;

MAKE_HOOK_MATCH(DefaultScenesTransitionsFromInit_TransitionToNextScene, &GlobalNamespace::DefaultScenesTransitionsFromInit::TransitionToNextScene, void, GlobalNamespace::DefaultScenesTransitionsFromInit* self, bool goStraightToMenu, bool goStraightToEditor, bool goToRecordingToolScene)
{
    DefaultScenesTransitionsFromInit_TransitionToNextScene(self, goStraightToMenu, goStraightToEditor, goToRecordingToolScene);
    Cinema::PlaybackController::Create();
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
    DEBUG("Paper Test Log");
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
}

#include "bsml/shared/BSMLDataCache.hpp"

BSML_DATACACHE(settings)
{
    return IncludedAssets::VideoMenu_bsml;
}