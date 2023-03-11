#include "WIPUI/VideoMenuManager.hpp"
#include "ModConfig.hpp"


// will remove QUI at some point
#include "questui/shared/BeatSaberUI.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/ViewController_AnimationType.hpp"

// Song Downloading
#include "pythonlib/shared/Python.hpp"
#include "pythonlib/shared/Utils/FileUtils.hpp"
#include "pythonlib/shared/Utils/StringUtils.hpp"
#include "assets.hpp"

DEFINE_TYPE(Cinema, VideoMenuManager);

namespace Cinema
{
    SafePtr<VideoMenuManager> VideoMenuManager::instance;
    VideoMenuManager *VideoMenuManager::get_instance()
    {
        if (!instance)
            instance = VideoMenuManager::New_ctor();
        return instance.ptr();
    }

    bool VideoMenuManager::DownloadCurrentVideo()
    {}

    bool VideoMenuManager::DownloadVideo(std::string_view url, std::string_view filepath, std::function<void(float)> status)
    {
        bool error = false;
        std::function<void(int, char*)> eventHandler = [status, &error](int type, char* data) {
            switch (type) {
            case 0:
                {
                    std::string dataString(data);
                    if(dataString.find("[download]", 0) != -1) {
                        auto pos = dataString.find("%", 0);
                        if(pos != -1 && pos > 5) {
                            auto percentange = dataString.substr(pos-5, 5);
                            if(percentange.find("]", 0) == 0) 
                                percentange = percentange.substr(1);
                            status(std::stof(percentange));
                        }
                    }
                }
                break;
            case 1:
                error = true;
                getLogger().info("Error: %s", data);
                break;
            }
        };
        Python::PythonWriteEvent += eventHandler;
        std::string ytdlp = FileUtils::getScriptsPath() + "/yt_dlp";
        if(!direxists(ytdlp))
            FileUtils::ExtractZip(IncludedAssets::ytdlp_zip, ytdlp);
        Python::PyRun_SimpleString("from yt_dlp.__init__ import _real_main");
        std::string command = "_real_main([";
        for(auto splitted : StringUtils::Split("--no-cache-dir -o %(id)s.%(ext)s -P /sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema " + url, " ")) {
            command += "\"" + splitted + "\",";
        }
        command = command.substr(0, command.length()-1) + "])";
        int result = Python::PyRun_SimpleString(command.c_str());
        Python::PythonWriteEvent -= eventHandler;
        return !error;
    }


    void VideoMenuManager::ctor() { }

    bool VideoMenuManager::get_doesCurrentSongUseCinema() { return this->doesCurrentSongUseCinema; }
    void VideoMenuManager::set_doesCurrentSongUseCinema(bool value) 
    {
        this->doesCurrentSongUseCinema = value;
        // Update UI
    }

    bool VideoMenuManager::get_isCurrentSongDownloaded() { return this->isCurrentSongDownloaded; }
    void VideoMenuManager::set_isCurrentSongDownloaded(bool value)
    {
        this->isCurrentSongDownloaded = value;
        // Update UI
    }
}