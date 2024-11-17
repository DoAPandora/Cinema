#pragma once

#include "Settings/VideoQuality.hpp"
#include "Video/VideoConfig.hpp"

#include <string>
#include <memory>

namespace Cinema::VideoQuality
{
    enum class Mode {
        Q1080P = 1080,
        Q720P = 720,
        Q480P = 480,
    };

    std::string ToString(Mode mode);

    std::string ToYoutubeDLFormat(std::shared_ptr<VideoConfig> video, Mode quality);

    Mode FromName(std::string& mode);
}