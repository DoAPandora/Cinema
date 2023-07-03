#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-experimental-coroutine"

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
                getLogger().info("Error: %s", data);
                break;
            }
        };
        Python::PythonWriteEvent += eventHandler;
        Python::PyRun_SimpleString("from yt_dlp.__init__ import _real_main");
        std::string command = "_real_main([";
        for(auto splitted : StringUtils::Split("--no-cache-dir -o %(id)s.%(ext)s -P /sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Videos " + url, " ")) {
            command += "\"" + splitted + "\",";
        }
        command = command.substr(0, command.length()-1) + "])";
        int result = Python::PyRun_SimpleString(command.c_str());
        Python::PythonWriteEvent -= eventHandler;
        return !error;
    }

    custom_types::Helpers::Coroutine DownloadThumbnail(std::string_view id, std::function<void()> onFinished)
    {
        
        auto www = UnityEngine::Networking::UnityWebRequest::Get("https://i.ytimg.com/vi/" + id + "/hqdefault.jpg");
        www->set_timeout(3);
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(www->SendWebRequest());
        
        if (!www->get_isNetworkError() && !www->get_isHttpError()) {
            std::ofstream f("/sdcard/ModData/com.beatgames.beatsaber/Mods/Cinema/Thumbnails/" + id + ".jpg",  std::ios_base::binary | std::ios_base::trunc);
            auto arr = www->get_downloadHandler()->GetData();
            f.write((char*)arr.begin(), arr.size());
            f.flush();
            f.close();

            if (onFinished)
                onFinished();
        } else {
            getLogger().info("Getting thumbnail failed");
        }

        co_return;
    }
}
#pragma clang diagnostic pop