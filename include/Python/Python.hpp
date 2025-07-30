#pragma once

#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"

#include <string>
#include <string_view>
#include <filesystem>
#include <queue>
#include <future>
#include <mutex>
#include <thread>

struct _object;
typedef struct _object PyObject;

namespace Cinema
{
    class Python
    {
    public:
        static const std::filesystem::path PYTHON_HOME_DIRECTORY;
        static const std::filesystem::path SCRIPTS_DIRECTORY;

        // May throw on error
        static void Setup();
        static bool IsReady();
        static std::future<int> QueueCommand(std::string_view command);

        static UnorderedEventCallback<std::string_view> StandardOutputWriteEvent;
        static UnorderedEventCallback<std::string_view> StandardErrorWriteEvent;

        static PyObject* nativelib_std_write(PyObject* self, PyObject* args);

    private:

        static void InstallPrefix();
        static void InstallOutputRedirect();
        static void PythonThread();

        using CommandQueueEntry = std::pair<std::string, std::promise<int>>;
    
        static std::queue<CommandQueueEntry> commandQueue;
        static std::mutex commandQueueMutex;
        static std::jthread pythonExecutionThread;
        static bool isReady;
    
        Python() = delete;
    };
}