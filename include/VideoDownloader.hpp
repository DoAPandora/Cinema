#include <string>

namespace Cinema {
    void DownloadVideo(std::string_view url, std::string_view filepath, std::function<void(float)> status = nullptr, std::function<void(bool)> onFinished = nullptr);
}