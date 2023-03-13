#include "main.hpp"
#include "hooks.hpp"
#include "ModConfig.hpp"
#include "BSML/shared/BSML.hpp"

#include "UI/VideoMenuManager.hpp"
#include "assets.hpp"
#include "pinkcore/shared/RequirementAPI.hpp"

using namespace UnityEngine;
using namespace GlobalNamespace;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

ModInfo& getModInfo() {
    return modInfo;
}

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing hooks...");
    Cinema::Hooks::InstallVideoPlayerHooks();
    Cinema::Hooks::InstallVideoDownloadHooks();
    getLogger().info("Installed all hooks!");

    getModConfig().Init(modInfo);

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