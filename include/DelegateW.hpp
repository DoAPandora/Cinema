/*
 * Made by marmott
 * https://github.com/Royston1999
 */

#pragma once
#include "custom-types/shared/delegate.hpp"

template <typename T>
struct delegate_func;

template <typename TRet, typename T, typename... TArgs>
struct delegate_func<TRet(T::*)(TArgs...)>
{
    using FuncType = TRet(TArgs...);
};

template<typename T>
using delegate_func_t = typename delegate_func<decltype(&T::Invoke)>::FuncType;

template <typename O, typename T>
struct delegate_mem_func;

template <typename O, typename TRet, typename T, typename... TArgs>
struct delegate_mem_func<O, TRet(T::*)(TArgs...)>
{
    using MemFuncPtr = TRet(O::*)(TArgs...);
};

template<typename O, typename T>
using delegate_mem_func_t = typename delegate_mem_func<O, decltype(&T::Invoke)>::MemFuncPtr;

template <std::size_t... I>
auto make_placeholders(std::index_sequence<I...>)
{
    return std::make_tuple(std::placeholders::__ph<I+1>{}...);
}

template <typename TRet, typename TName, typename... TArgs, typename TObj>
const auto bind(TRet(TName::*fp)(TArgs...), TObj obj)
{
    constexpr int argc = sizeof...(TArgs);
    if constexpr (argc == 0) return std::bind(fp, obj);
    auto paramPack = std::tuple_cat(std::make_tuple(fp, obj), make_placeholders(std::make_index_sequence<argc>()));
    return std::apply([](auto &&... args) { return std::bind(args...); }, paramPack);
}

template<typename T>
struct DelegateW
{
    static_assert(std::is_convertible_v<T*, System::MulticastDelegate*>, "T must be a delegate type");
    DelegateW() {}
    DelegateW(std::function<delegate_func_t<T>> function)
    {
        this->internalCSharpDelegate = custom_types::MakeDelegate<T*>(function);
    }
    DelegateW(const DelegateW<T>& other) : internalCSharpDelegate(other.internalCSharpDelegate) {}

    template<typename O>
    DelegateW(delegate_mem_func_t<O,T> function, O* object)
    {
        this->internalCSharpDelegate = custom_types::MakeDelegate<T*>(std::function<delegate_func_t<T>>(bind(function, object)));
    }

    DelegateW<T>& operator=(DelegateW<T>&& other)
    {
        this->internalCSharpDelegate = other.internalCSharpDelegate ? other.internalCSharpDelegate.ptr() : nullptr;
        return *this;
    }
    DelegateW<T>& operator=(std::function<delegate_func_t<T>> function)
    {
        this->internalCSharpDelegate = custom_types::MakeDelegate<T*>(function);
        return *this;
    }
    constexpr SafePtr<T>* operator->() noexcept { return &internalCSharpDelegate; }
    constexpr SafePtr<T> const* operator->() const noexcept { return &internalCSharpDelegate; }
    constexpr operator bool() const noexcept { return internalCSharpDelegate; }

    private:
    SafePtr<T> internalCSharpDelegate;
};

template<typename T>
requires (std::is_convertible_v<T*, System::MulticastDelegate*>)
void operator+=(T*& csDelegate, DelegateW<T> delegateW)
{
    if (!delegateW) return;
    T* newDelegate = reinterpret_cast<T*>(System::Delegate::Combine(csDelegate, delegateW->ptr()));
    csDelegate = newDelegate;
}

template<typename T>
requires (std::is_convertible_v<T*, System::MulticastDelegate*>)
void operator-=(T*& csDelegate, DelegateW<T>& delegateW)
{
    if (!delegateW) return;
    T* newDelegate = reinterpret_cast<T*>(System::Delegate::Remove(csDelegate, delegateW->ptr()));
    csDelegate = newDelegate;
}