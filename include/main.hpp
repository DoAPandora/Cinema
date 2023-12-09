#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "modloader/shared/modloader.hpp"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "hooking.hpp"

#include "ModConfig.hpp"

// Define these functions here so that we can easily read configuration and log information from other files
Logger& getLogger();
extern ModInfo modInfo;

#define VIDEO_DIR "/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Videos/"
#define THUMBNAIL_DIR "/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Thumbnails/"

#include "logger.hpp"