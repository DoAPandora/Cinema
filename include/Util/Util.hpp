#pragma once

#include <string>

namespace Cinema::Util {

    std::string ReplaceIllegalFilesystemChar(const std::string& s);
    std::string GetEnvironmentName();
}