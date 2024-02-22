#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Mesh.hpp"

#define MIN_CURVATURE 0.0001f
#define SUBSURFACE_COUNT_DEFAULT 32
#define SUBSURFACE_COUNT_MIN 1
#define SUBSURFACE_COUNT_MAX 512

DECLARE_CLASS_CODEGEN(Cinema, CurvedSurface, UnityEngine::MonoBehaviour,

    struct MeshData {
        ArrayW<UnityEngine::Vector3> vertices;
        ArrayW<UnityEngine::Vector2> UVs;
        ArrayW<int> triangles;
    };

    DECLARE_INSTANCE_FIELD_PRIVATE(float, distance);
    DECLARE_INSTANCE_FIELD_PRIVATE(float, height);
    DECLARE_INSTANCE_FIELD_PRIVATE(float, width);
    DECLARE_INSTANCE_FIELD(float, radius);
    DECLARE_INSTANCE_FIELD(int, subsurfaceCount);
    DECLARE_INSTANCE_FIELD(bool, curveYAxis);
    DECLARE_INSTANCE_FIELD(float, curvatureDegreesAutomatic);

public:

    std::optional<float> curvatureDegreesFixed;

    DECLARE_INSTANCE_METHOD(void, Start);
    DECLARE_INSTANCE_METHOD(void, Update);
    DECLARE_INSTANCE_METHOD(void , UpdateRadius);
    DECLARE_INSTANCE_METHOD(float, GetCircleFraction);
    DECLARE_INSTANCE_METHOD(void, Generate);

public:

    void Initialise(float width, float height, float distance, std::optional<float> curvatureDegrees, std::optional<int> subsurfaces, std::optional<bool> curveYAxis);
    MeshData CreateSurface();
    void UpdateMeshFilter(MeshData& surface);
    void GenerateVertexPair(MeshData& surface, int i);
    void ConnectVertices(MeshData& surface, int i, int& j) const;

    DECLARE_INSTANCE_METHOD(void, set_distance, float value);
    DECLARE_INSTANCE_METHOD(float, get_distance);
    DECLARE_INSTANCE_METHOD(void, set_width, float value);
    DECLARE_INSTANCE_METHOD(float, get_width);
    DECLARE_INSTANCE_METHOD(void, set_height, float value);
    DECLARE_INSTANCE_METHOD(float, get_height);
    DECLARE_INSTANCE_METHOD(float, get_curvatureDegrees);

public:

    __declspec(property(get=get_distance, put=set_distance)) float Distance;
    __declspec(property(get=get_width, put=set_width)) float Width;
    __declspec(property(get=get_height, put=set_height)) float Height;
    __declspec(property(get=get_curvatureDegrees)) float CurvatureDegrees;

    )