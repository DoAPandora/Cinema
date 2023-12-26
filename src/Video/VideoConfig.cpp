#include "main.hpp"
#include "Video/VideoConfig.hpp"
#include "Util/Util.hpp"


namespace Cinema::VideoLoader {
    extern std::string GetConfigPath(const std::string& levelPath);
}

namespace Cinema {

    bool VideoConfig::get_IsWIPLevel() const
    {
        return levelDir != std::nullopt && levelDir->find("CustomWIPLevels") != std::string::npos;
    }

    bool VideoConfig::get_IsOfficialConfig() const
    {
        return configByMapper.value_or(false);
    }

    bool VideoConfig::get_TransparencyEnabled() const
    {
        return ((transparency == std::nullopt) || transparency != std::nullopt && !transparency.value());
    }

    bool VideoConfig::get_IsDownloading() const
    {
        return downloadState == DownloadState::Preparing ||
               downloadState == DownloadState::Downloading ||
               downloadState == DownloadState::DownloadingVideo ||
               downloadState == DownloadState::DownloadingAudio;
    }

    std::optional<std::string> VideoConfig::get_VideoPath()
    {
        if (levelDir != std::nullopt && IsWIPLevel)
        {
            std::filesystem::path levelPath(*levelDir);
            auto path = levelPath.parent_path();
            auto mapFolderName = levelPath.filename().string();

            DEBUG("Found Parent folder {}", path.string());
            auto folder = path / "CinemaWIPVideos" / mapFolderName;
            videoFile = GetVideoFileName(folder);
            return folder / *videoFile;
        }

        if (levelDir != std::nullopt)
        {
            try
            {
                videoFile = GetVideoFileName(*levelDir);
                return std::filesystem::path(*levelDir) / *videoFile;
            }
            catch (const std::exception& e)
            {
                ERROR("Failed to combine video path for {}: {}", *videoFile, e.what());
                return std::nullopt;
            }
        }

        DEBUG("VideoPath is null");
        return std::nullopt;
    }

    std::optional<std::string> VideoConfig::get_ConfigPath() const
    {
        if(levelDir.has_value())
        {
            return VideoLoader::GetConfigPath(*levelDir);
        }
        return std::nullopt;
    }

    bool VideoConfig::get_isPlayable() const { return true; }

    std::string VideoConfig::GetVideoFileName(std::string levelPath) const
    {
        std::string fileName = videoFile.value_or(Cinema::Util::ReplaceIllegalFilesystemChar(title.value_or(videoID.value_or("video"))));
        // shorten
        if (!fileName.ends_with(".mp4"))
        {
            fileName.append(".mp4");
        }
        return fileName;
    }

    std::string VideoConfig::ToString() const
    {
        return fmt::format("[{}] {} by {} ({})", videoID, title, author, duration);
    }

    float VideoConfig::GetOffsetInSec() const
    {
        return offset / 1000.0f;
    }

    DownloadState VideoConfig::UpdateDownloadState()
    {
        return (downloadState = (VideoPath != std::nullopt && (videoID != std::nullopt || videoUrl != std::nullopt) && std::filesystem::exists(*VideoPath) ? DownloadState::Downloaded : DownloadState::NotDownloaded));
    }
}