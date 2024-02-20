#include "main.hpp"
#include "Downloader.hpp"

#include "pythonlib/shared/Python.hpp"
#include "pythonlib/shared/Utils/StringUtils.hpp"

#include "UnityEngine/Networking/DownloadHandler.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/UnityWebRequestAsyncOperation.hpp"
#include "UnityEngine/Networking/UnityWebRequestTexture.hpp"
#include "UnityEngine/Networking/DownloadHandlerTexture.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/Texture2D.hpp"

#include <fstream>

namespace Cinema::Downloader {

    bool DownloadVideo(std::string_view url, std::function<void(float)> status)
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
                break;
            }
        };
        Python::PythonWriteEvent += eventHandler;
        Python::PyRun_SimpleString("from yt_dlp.__init__ import _real_main");
        std::string commandFormatted = "_real_main([";
    
        std::string command{"--no-cache-dir -o %(id)s.%(ext)s -P /sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Videos "};
        command.append(url);
        for(auto splitted : StringUtils::Split(command, " ")) {
            commandFormatted += "\"" + splitted + "\",";
        }
        commandFormatted = commandFormatted.substr(0, command.length()-1) + "])";
        int result = Python::PyRun_SimpleString(commandFormatted.c_str());
        Python::PythonWriteEvent -= eventHandler;
        return !error;
    }

    custom_types::Helpers::Coroutine DownloadThumbnail(std::string_view id, std::function<void()> onFinished)
    {
        std::string url{"https://i.ytimg.com/vi/"};
        url.append(id);
        url.append("/hqdefault.jpg");

        auto www = UnityEngine::Networking::UnityWebRequest::Get(url);
        www->set_timeout(3);
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(www->SendWebRequest());
        
        if (www->get_result() != UnityEngine::Networking::UnityWebRequest::Result::Success) {
            std::string filepath{"/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Thumbnails/"};
            filepath.append(id);
            filepath.append(".jpg");

            std::ofstream f(url,  std::ios_base::binary | std::ios_base::trunc);
            auto arr = www->get_downloadHandler()->GetData();
            f.write((char*)arr.begin(), arr.size());
            f.flush();
            f.close();

            if (onFinished)
                onFinished();
        } else {
            ERROR("Getting thumbnail failed");
        }

        co_return;
    }
}