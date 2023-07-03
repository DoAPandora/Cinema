#include "main.hpp"
#include "Hooks/Hooks.hpp"

#include "BSML/shared/BSML.hpp"

#include "Screen/PlaybackController.hpp"

#include "UI/VideoMenuManager.hpp"
#include "assets.hpp"
#include "pinkcore/shared/RequirementAPI.hpp"

#include "GlobalNamespace/DefaultScenesTransitionsFromInit.hpp"

#include "pythonlib/shared/Utils/FileUtils.hpp"

static ModInfo modInfo;

MAKE_HOOK_MATCH(DefaultScenesTransitionsFromInit_TransitionToNextScene, &GlobalNamespace::DefaultScenesTransitionsFromInit::TransitionToNextScene, void, GlobalNamespace::DefaultScenesTransitionsFromInit* self, bool goStraightToMenu, bool goStraightToEditor, bool goToRecordingToolScene)
{
    DefaultScenesTransitionsFromInit_TransitionToNextScene(self, goStraightToMenu, goStraightToEditor, goToRecordingToolScene);
    Cinema::PlaybackController::Create();
}

ModInfo& getModInfo() {
    return modInfo;
}

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	
    getModConfig().Init(modInfo);
    getLogger().info("Completed setup!");
}


// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    mkpath(videosDir);
    mkpath(thumbnailsDir);

    getLogger().info("Installing hooks...");
    Cinema::Hooks::InstallPlaybackControllerEventHooks();
    Cinema::Hooks::InstallVideoDownloadHooks();
    Cinema::Hooks::InstallLevelDataHook();
    INSTALL_HOOK(getLogger(), DefaultScenesTransitionsFromInit_TransitionToNextScene);
    getLogger().info("Installed all hooks!");

    std::string ytdlp = FileUtils::getScriptsPath() + "/yt_dlp";
    if(!direxists(ytdlp))
        FileUtils::ExtractZip(IncludedAssets::ytdlp_zip, ytdlp);

    BSML::Init();
    BSML::Register::RegisterGameplaySetupTab("Cinema", MOD_ID "_settings", Cinema::VideoMenuManager::get_instance(), BSML::MenuType::Solo);

    PinkCore::RequirementAPI::RegisterInstalled("Cinema");

	custom_types::Register::AutoRegister();
}

#include "bsml/shared/BSMLDataCache.hpp"

BSML_DATACACHE(settings)
{
    return IncludedAssets::VideoMenu_bsml;
}