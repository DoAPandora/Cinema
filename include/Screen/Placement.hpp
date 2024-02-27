#pragma once

#include "UnityEngine/Vector3.hpp"

#include <optional>

namespace Cinema
{
    struct Placement
    {
    public:

        Placement(UnityEngine::Vector3 _position, UnityEngine::Vector3 _rotation, float _height, std::optional<float> _width = std::nullopt, std::optional<float> _curvature = std::nullopt):
        position(_position), rotation(_rotation), height(_height), width(_width.value_or(_height * (16.0f / 9.0f))), curvature(_curvature) {}

        UnityEngine::Vector3 position;
        UnityEngine::Vector3 rotation;
        float height;
        float width;
        std::optional<float> curvature;
        std::optional<bool> curveYAxis;
        std::optional<int> subsurfaces;

        // defined in main.cpp
        static Placement MenuPlacement;

    };
}