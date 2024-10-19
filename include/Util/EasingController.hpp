#pragma once

#include "custom-types/shared/coroutine.hpp"

#include "UnityEngine/Coroutine.hpp"

namespace Cinema
{
    class EasingController
    {
        enum class EasingDirection : int8_t
        {
            EaseIn = 1,
            EaseOut = -1
        };

        float easingValue;
        UnityEngine::Coroutine* easingCoroutine;
        bool isFading;

        static inline const float DEFAULT_DURATION = 1;

    public:

        UnorderedEventCallback<float> easingUpdate;
        
        EasingController(float initialValue = 0): easingValue(initialValue)
        {}

        void EaseIn(float duration = DEFAULT_DURATION);
        void EaseOut(float duration = DEFAULT_DURATION);

        bool IsFading();
        bool IsOne();
        bool IsZero();
        __declspec(property(get=get_value, put=set_value)) float value;

        float get_value();
        void set_value(float value);

    private:

        void StartEasingCoroutine(EasingDirection direction, float duration);
        custom_types::Helpers::Coroutine Ease(float speed);
    };
}