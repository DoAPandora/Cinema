#include "main.hpp"
#include "Screen/CurvedSurface.hpp"

#include "UnityEngine/Mathf.hpp"
#include "UnityEngine/MeshFilter.hpp"

#include <cmath>
#include <limits>

DEFINE_TYPE(Cinema, CurvedSurface);

#define DEG2RAD 0.017453292f

using namespace UnityEngine;

namespace Cinema {

    void CurvedSurface::Initialise(float width, float height, float distance, std::optional<float> curvatureDegrees, std::optional<int> subsurfaces, std::optional<bool> curveYAxis)
   {
       if (curvatureDegrees.has_value())
       {
           curvatureDegrees = std::clamp(curvatureDegrees.value(), MIN_CURVATURE, 360.0f);
       }

       curvatureDegreesFixed = curvatureDegrees;

       subsurfaceCount = subsurfaces.value_or(SUBSURFACE_COUNT_DEFAULT);
       subsurfaceCount = std::clamp(subsurfaceCount, SUBSURFACE_COUNT_MIN, SUBSURFACE_COUNT_MAX);

       this->curveYAxis = curveYAxis.value_or(false);

       this->width = width;
       this->height = height;
       this->distance = distance;
       UpdateRadius();
   }

    void CurvedSurface::Update()
    {
        auto dist = Vector3::Distance(get_transform()->get_position(), Vector3::get_zero());
        if (abs(dist - this->distance) < 0.2f)
            return;
        Distance = dist;
    }

    void CurvedSurface::UpdateRadius()
    {
        auto length = curveYAxis ? Height : Width;
        curvatureDegreesAutomatic = MIN_CURVATURE;
        if (curvatureDegreesFixed.has_value())
        {
            radius = (float) (GetCircleFraction() / (2 * M_PI)) * length;
        }
        else
        {
            radius = Distance;
            curvatureDegreesAutomatic = (float) (360/(((2 * M_PI) * radius) / length));
        }
    }

    float CurvedSurface::GetCircleFraction()
    {
        auto circleFraction = std::numeric_limits<float>::max();
        if (CurvatureDegrees > 0)
        {
            circleFraction = 360.0f / CurvatureDegrees;
        }
        return circleFraction;
    }

    void CurvedSurface::Generate()
    {
        auto surface = CreateSurface();
        UpdateMeshFilter(surface);
    }

    CurvedSurface::MeshData CurvedSurface::CreateSurface()
    {
        MeshData surface
        {
            .vertices = ArrayW<Vector3>((subsurfaceCount + 2) * 2),
            .UVs = ArrayW<Vector2>((subsurfaceCount + 2) * 2),
            .triangles = ArrayW<int>(subsurfaceCount * 6)
        };

        int i, j;
        for (i = j = 0; i < subsurfaceCount + 1; i++)
        {
            GenerateVertexPair(surface, i);

            if (i >= subsurfaceCount)
            {
                continue;
            }
            ConnectVertices(surface, i, j);
        }

        return surface;
    }

    void CurvedSurface::UpdateMeshFilter(Cinema::CurvedSurface::MeshData &surface)
    {
        auto filter = GetComponent<MeshFilter*>();

        auto mesh = Mesh::New_ctor();
        mesh->set_vertices(surface.vertices);
        mesh->set_triangles(surface.triangles);
        mesh->SetUVs(0, surface.UVs);
        filter->set_mesh(mesh);
    }

    void CurvedSurface::GenerateVertexPair(Cinema::CurvedSurface::MeshData &surface, int i)
    {
        auto segmentDistance = ((float)i) / subsurfaceCount;
        auto arcDegrees = CurvatureDegrees * DEG2RAD;
        auto theta = -0.5f + segmentDistance;


        auto z = (cos(theta * arcDegrees) * radius) - radius;
        auto uv = i / (float) subsurfaceCount;

        if (curveYAxis)
        {
            auto x = Width / 2.0f;
            auto y = sin(theta * arcDegrees) * radius;
            surface.vertices[i + subsurfaceCount + 1] = Vector3(x, y, z);
            surface.vertices[i] = Vector3(-x, y, z);
            surface.UVs[i + subsurfaceCount + 1] = Vector2(1, uv);
            surface.UVs[i] = Vector2(0, uv);
        }
        else
        {
            auto x = sin(theta * arcDegrees)  * radius;
            auto y = Height / 2.0f;
            surface.vertices[i] = Vector3(x, y, z);
            surface.vertices[i + subsurfaceCount + 1] = Vector3(x, -y, z);
            surface.UVs[i] = Vector2(uv, 1);
            surface.UVs[i + subsurfaceCount + 1] = Vector2(uv, 0);
        }
    }

    void CurvedSurface::ConnectVertices(Cinema::CurvedSurface::MeshData &surface, int i, int &j) const
    {
        //Left triangle
        surface.triangles[j++] = i;
        surface.triangles[j++] = i + 1;
        surface.triangles[j++] = i + subsurfaceCount + 1;

        //Right triangle
        surface.triangles[j++] = i + 1;
        surface.triangles[j++] = i + subsurfaceCount + 2;
        surface.triangles[j++] = i + subsurfaceCount + 1;
    }


    float CurvedSurface::get_distance() { return distance; }
    void CurvedSurface::set_distance(float value) { distance = value; UpdateRadius(); }

    float CurvedSurface::get_width() { return width; }
    void CurvedSurface::set_width(float value) { width = value; UpdateRadius(); }

    float CurvedSurface::get_height() { return height; }
    void CurvedSurface::set_height(float value) { height = value; UpdateRadius(); }

    float CurvedSurface::get_curvatureDegrees() { return curvatureDegreesFixed.value_or(curvatureDegreesAutomatic); }
}