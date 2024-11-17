#include "Settings/VideoQuality.hpp"

#include <memory>
#include <string>
#include <stdexcept>
#include <regex>

namespace Cinema::VideoQuality
{
    std::string ToString(Mode mode)
    {
        return std::to_string((int)mode).append("p");
    }

    std::string ToYoutubeDLFormat(std::shared_ptr<VideoConfig> video, Mode quality)
    {
        std::string qualityString;
        if(!video->videoUrl.has_value() || video->videoUrl->starts_with("https://www.youtube.com/watch"))
        {
            qualityString = fmt::format("bestvideo[height<={}][vcodec*=avc1]+bestaudio[acodec*=mp4]", (int)quality);
        } 
        else if(video->videoUrl->starts_with("https://vimeo.com/"))
        {
            qualityString = fmt::format("bestvideo[height<={}][vcodec*=avc1]+bestaudio[acodec*=mp4]", (int)quality);
        }
        else if(video->videoUrl->starts_with("https://www.bilibili.com"))
        {
            qualityString = fmt::format("bestvideo[height<={}][vcodec*=avc1]+bestaudio[acodec*=mp4]", (int)quality);
        }
        else if(video->videoUrl->starts_with("https://www.facebook.com"))
        {
            qualityString = "mp4";
        }
        else
        {
            qualityString = fmt::format("best[height<={}][vcodec*=avc1]", (int)quality);
        }

        return qualityString;
    }

    Mode FromName(std::string& mode)
    {
        std::regex reg("([0-9]{3,4})p.*");
        std::smatch match;
        if(!std::regex_search(mode, match, reg) || match.size() == 0)
        {
            throw std::invalid_argument(fmt::format("Cannot extract QualityMode from download URL! ({})", mode));
        }

        return (Mode)std::stoi(match[1]);
    }
}