#include "Util/EasingController.hpp"

#include "bsml/shared/BSML/SharedCoroutineStarter.hpp"
#include "UnityEngine/Time.hpp"

#include <algorithm>
#include <cstdlib>

namespace Cinema
{
    bool EasingController::IsOne()
    {
        return std::abs(easingValue - 1) < 0.00001f;
    }

    bool EasingController::IsZero()
    {
        return easingValue == 0;
    }

    void EasingController::set_value(float value)
    {
        easingUpdate.invoke(value);
        easingValue = value;
    }

    float EasingController::get_value()
    {
        return easingValue;
    }

    bool EasingController::IsFading()
    {
        return isFading;
    }

    void EasingController::EaseIn(float duration)
    {
        StartEasingCoroutine(EasingDirection::EaseIn, duration);
    }

    void EasingController::EaseOut(float duration)
    {
        StartEasingCoroutine(EasingDirection::EaseOut, duration);
    }

    void EasingController::StartEasingCoroutine(EasingDirection direction, float duration)
    {
        if(easingCoroutine && easingCoroutine->m_Ptr.m_value)
        {
            BSML::SharedCoroutineStarter::StopCoroutine(easingCoroutine);
        }

        isFading = true;
        float speed = (int) direction / (float) std::max(0.0001f, duration);

        easingCoroutine = BSML::SharedCoroutineStarter::StartCoroutine(Ease(speed));
    }

    custom_types::Helpers::Coroutine EasingController::Ease(float speed)
    {
        do {
            easingValue += UnityEngine::Time::get_deltaTime() * speed;
            easingValue = std::clamp<float>(easingValue, 0, 1);
            easingUpdate.invoke(easingValue);
            co_yield nullptr;
        } while (easingValue > 0 && easingValue < 1);

        isFading = false;
        co_return;
    }
}