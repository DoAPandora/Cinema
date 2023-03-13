#include <string>

namespace Cinema::Downloader {
    bool DownloadVideo(std::string_view url, std::function<void(float)> status = nullptr);

    bool DownloadThumbnail(std::string_view url, std::function<void(float)> status = nullptr);
}