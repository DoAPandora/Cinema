#pragma once

// Include the modloader header, which allows us to tell the modloader which mod this is, and the version etc.
#include "scotland2/shared/modloader.h"

// beatsaber-hook is a modding framework that lets us call functions and fetch field values from in the game
// It also allows creating objects, configuration, and importantly, hooking methods to modify their values
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"

#include "ModConfig.hpp"

// Define these functions here so that we can easily read configuration and log information from other files
extern modloader::ModInfo modInfo;

#define VIDEO_DIR "/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Videos/"
#define THUMBNAIL_DIR "/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Thumbnails/"

#include "logger.hpp"
// Relies on include order :(
#include "hooking.hpp"

#define CINEMA_EXPORT extern "C" __attribute__((visibility("default")))