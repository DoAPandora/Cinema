#pragma once

#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Action.hpp"

#include "custom-types/shared/delegate.hpp"

namespace DelegateHelper {

    template <typename T>
    void ContinueWith(System::Threading::Tasks::Task_1<T>* task, const std::function<void(T)>& callback)
    {
        auto delegate = custom_types::MakeDelegate<System::Action*>(std::function([callback, task]()
                {
                    callback(task->get_Result());
                }));
        task->GetAwaiter().OnCompleted(delegate);
    }
}