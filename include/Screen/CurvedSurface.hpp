#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Mesh.hpp"

#define MIN_CURVATURE 0.0001f
#define SUBSURFACE_COUNT_DEFAULT 32
#define SUBSURFACE_COUNT_MIN 1
#define SUBSURFACE_COUNT_MAX 512

DECLARE_CLASS_CODEGEN(Cinema, CurvedSurface, UnityEngine::MonoBehaviour) {

    struct MeshData {
        ArrayW<UnityEngine::Vector3> vertices;
        ArrayW<UnityEngine::Vector2> UVs;
        ArrayW<int> triangles;
    };

    float distance;
    float height;
    float width;
    float radius;
    int subsurfaceCount;
    bool curveYAxis;
    float curvatureDegreesAutomatic;
    std::optional<float> curvatureDegreesFixed;

public:

    __declspec(property(get=get_distance, put=set_distance)) float Distance;
    __declspec(property(get=get_width, put=set_width)) float Width;
    __declspec(property(get=get_height, put=set_height)) float Height;
    __declspec(property(get=get_curvatureDegrees)) float CurvatureDegrees;

    DECLARE_INSTANCE_METHOD(void, Update);
    
public:

    void UpdateRadius();
    float GetCircleFraction();
    void Generate();

    void Initialise(float width, float height, float distance, std::optional<float> curvatureDegrees, std::optional<int> subsurfaces, std::optional<bool> curveYAxis);
    MeshData CreateSurface();
    void UpdateMeshFilter(MeshData& surface);
    void GenerateVertexPair(MeshData& surface, int i);
    void ConnectVertices(MeshData& surface, int i, int& j) const;

    void set_distance(float value);
    float get_distance();
    void set_width(float value);
    float get_width();
    void set_height(float value);
    float get_height();
    float get_curvatureDegrees();
};