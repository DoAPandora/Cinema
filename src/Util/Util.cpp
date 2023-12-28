#include "Util/Util.hpp"

#include <regex>

/*
 * https://cs.android.com/android/platform/superproject/+/master:frameworks/base/core/java/android/os/FileUtils.java;l=997?q=isValidFatFilenameChar
 */
#define INVALID_FILENAME_CHARS "\"\\*\\:\\<\\>\\?\\\\|"

namespace Cinema::Util {

    std::string ReplaceIllegalFilesystemChar(const std::string& s) {
        std::regex searchRegex("[" INVALID_FILENAME_CHARS "]");
        return std::regex_replace(s, searchRegex, "_");
    }
}