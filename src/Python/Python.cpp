#include "Python/Python.hpp"
#include "assets.hpp"
#include "logger.hpp"

#include "scotland2/shared/modloader.h"

#include "cpython/Python.h"

#include <exception>

#include "zip/src/zip.h"

namespace Cinema
{
    UnorderedEventCallback<std::string_view> Python::StandardOutputWriteEvent;
    UnorderedEventCallback<std::string_view> Python::StandardErrorWriteEvent;

    std::queue<Python::CommandQueueEntry> Python::commandQueue;
    std::mutex Python::commandQueueMutex;
    std::jthread Python::pythonExecutionThread;

    bool Python::isReady = false;

    const std::filesystem::path Python::PYTHON_HOME_DIRECTORY(modloader::get_files_dir() / "python" / "prefix");
    const std::filesystem::path Python::SCRIPTS_DIRECTORY(modloader::get_files_dir() / "python" / "scripts");

    void Python::Setup()
    {
        if(isReady)
        {
            return;
        }

        if(!std::filesystem::exists(SCRIPTS_DIRECTORY))
        {
            std::filesystem::create_directories(SCRIPTS_DIRECTORY);
        }

        pythonExecutionThread = std::jthread(Python::PythonThread);
        pythonExecutionThread.detach();

        isReady = true;
    }

    bool Python::IsReady()
    {
        return isReady;
    }

    std::future<int> Python::QueueCommand(std::string_view command)
    {
        std::lock_guard lock(commandQueueMutex);
        return commandQueue.emplace(std::string(command), std::promise<int>()).second.get_future();
    }

    void Python::PythonThread()
    {
        DEBUG("Starting python thread");
        InstallPrefix();
        InstallOutputRedirect();

        auto getNextItem = [] -> std::optional<CommandQueueEntry> {
            std::lock_guard lock(commandQueueMutex);
            if(!commandQueue.empty())
            {
                auto val = std::move(commandQueue.front());
                commandQueue.pop();
                return val;
            }
            return std::nullopt;
        };

        while(true)
        {
            if(auto item = getNextItem(); item.has_value())
            {
                std::promise<int>& promise = item.value().second;
                std::string& command = item.value().first;
                DEBUG("Running command: {}", command);
                promise.set_value(PyRun_SimpleString(command.c_str()));
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

    void Python::InstallPrefix()
    {
        if(!std::filesystem::exists(PYTHON_HOME_DIRECTORY))
        {
            std::filesystem::create_directories(PYTHON_HOME_DIRECTORY);
        }

        int arg = 2;
        std::string_view data = IncludedAssets::prefix_zip;
        int res = zip_stream_extract(data.data(), data.size(), PYTHON_HOME_DIRECTORY.c_str(), nullptr, &arg);
        if(res != 0)
        {
            std::filesystem::remove_all(PYTHON_HOME_DIRECTORY);
            throw std::runtime_error(std::format("Failed to extract python prefix: {}", zip_strerror(res)));
        }

        const std::filesystem::path SSL_PATH = PYTHON_HOME_DIRECTORY / "etc" / "tls" / "cert.pem";
        setenv("PYTHONHOME", PYTHON_HOME_DIRECTORY.c_str(), 1);
        setenv("PYTHONPATH", SCRIPTS_DIRECTORY.c_str(), 1);
        setenv("SSL_CERT_FILE", SSL_PATH.c_str(), 1);

        Py_Initialize();

        INFO("Installed python prefix");
    }

    PyObject* Python::nativelib_std_write(PyObject* self, PyObject* args)
    {
        int type;
        char* data;
        if(PyArg_ParseTuple(args, "is", &type, &data))
        {
            switch(type)
            {
                case 1: 
                    StandardOutputWriteEvent.invoke(data);
                    break;
                case 2:
                    StandardErrorWriteEvent.invoke(data);
                    break;
            }
        }
        Py_RETURN_NONE;
    }

    static PyMethodDef NativeWriteMethods[] = {
        {"std_write", Python::nativelib_std_write, METH_VARARGS, ""},
        {nullptr, nullptr, 0, nullptr},
    };

    static PyModuleDef NativeWriteModule = {
        PyModuleDef_HEAD_INIT,
        "nativelib",
        nullptr,
        -1,
        NativeWriteMethods,
    };

    void Python::InstallOutputRedirect()
    {
        PyObject* mod = PyModule_Create(&NativeWriteModule);
        PyObject* sys_modules = PyImport_GetModuleDict();
        PyDict_SetItemString(sys_modules, NativeWriteModule.m_name, mod);
        Py_DecRef(mod);

        std::string_view data = IncludedAssets::redirect_std_py;
        PyRun_SimpleString(data.data());
        INFO("Loaded output redirect");
    }
}
