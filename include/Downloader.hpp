#include <string>
#include "custom-types/shared/coroutine.hpp"

namespace Cinema::Downloader {
    bool DownloadVideo(std::string_view url, std::function<void(float)> status = nullptr);

    custom_types::Helpers::Coroutine DownloadThumbnail(std::string_view id, std::function<void()> onFinished);
}