#pragma once

namespace UnityEngine::Rendering {
    enum class BlendMode : int32_t {
        Zero,
        One,
        DstColor,
        SrcColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcColor,
        DstAlpha,
        OneMinusDstAlpha,
        SrcAlphaSaturate,
        OneMinusSrcAlpha
    };
}