// batteries
#include "shape.h"
#include "vertex.h"

// glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace
{
  static void _init_gfx_resources(batteries::Shape *shape)
  {
    // initialize gfx resources
    shape->mesh.vertex_buffer = sg_make_buffer({
        .type = SG_BUFFERTYPE_VERTEXBUFFER,
        .data = {
            .ptr = shape->mesh.vertices.data(),
            .size = shape->mesh.vertices.size() * sizeof(batteries::vertex_t),
        },
        .label = "mesh-vertices",
    });
    shape->mesh.index_buffer = sg_make_buffer({
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = {
            .ptr = shape->mesh.indices.data(),
            .size = shape->mesh.indices.size() * sizeof(uint16_t),
        },
        .label = "mesh-indices",
    });
  }
}

// https://github.com/ewinebrenner/gpr300-sp24-template/blob/main/core/ew/procGen.cpp
namespace batteries
{
  static void createCubeFace(glm::vec3 normal, float size, Shape *shape)
  {
    auto startVertex = shape->mesh.vertices.size();
    auto a = glm::vec3(normal.z, normal.x, normal.y); // U axis
    auto b = glm::cross(normal, a);                   // V axis
    for (auto i = 0; i < 4; i++)
    {
      auto col = i % 2;
      auto row = i / 2;

      auto pos = normal * size * 0.5f;
      pos -= (a + b) * size * 0.5f;
      pos += (a * (float)col + b * (float)row) * size;

      auto uv = glm::vec2(col, row);

      shape->mesh.vertices.push_back({
          .position = {pos.x, pos.y, pos.z},
          .normal = {normal.x, normal.y, normal.z},
          .texcoord = {uv.x, uv.y},
      });

      // Indices
      shape->mesh.indices.push_back(startVertex);
      shape->mesh.indices.push_back(startVertex + 1);
      shape->mesh.indices.push_back(startVertex + 3);
      shape->mesh.indices.push_back(startVertex + 3);
      shape->mesh.indices.push_back(startVertex + 2);
      shape->mesh.indices.push_back(startVertex);
    }
  }

  Shape CreateCube(float size)
  {
    Shape shape;
    shape.mesh.vertices.reserve(24);                     // 6 x 4 vertices
    shape.mesh.indices.reserve(36);                      // 6 x 6 indices
    createCubeFace({+0.0f, +0.0f, +1.0f}, size, &shape); // Front
    createCubeFace({+1.0f, +0.0f, +0.0f}, size, &shape); // Right
    createCubeFace({+0.0f, +1.0f, +0.0f}, size, &shape); // Top
    createCubeFace({-1.0f, +0.0f, +0.0f}, size, &shape); // Left
    createCubeFace({+0.0f, -1.0f, +0.0f}, size, &shape); // Bottom
    createCubeFace({+0.0f, +0.0f, -1.0f}, size, &shape); // Back
    _init_gfx_resources(&shape);
    return shape;
  }

  Shape CreatePlane(float width, float height, int subdivisions)
  {
    Shape shape;
    // VERTICES
    for (auto row = 0; row <= subdivisions; row++)
    {
      for (auto col = 0; col <= subdivisions; col++)
      {
        shape.mesh.vertices.push_back({
            .position = {
                -width / 2 + width * ((float)col / subdivisions),
                0,
                height / 2 - height * ((float)row / subdivisions),
            },
            .normal = {0.0f, 1.0f, 0.0f},
            .texcoord = {((float)col / subdivisions), ((float)row / subdivisions)},
        });
      }
    }
    // INDICES
    auto columns = subdivisions + 1;
    for (auto row = 0; row < subdivisions; row++)
    {
      for (auto col = 0; col < subdivisions; col++)
      {
        auto start = row * columns + col;
        shape.mesh.indices.push_back(start);
        shape.mesh.indices.push_back(start + 1);
        shape.mesh.indices.push_back(start + columns + 1);
        shape.mesh.indices.push_back(start + columns + 1);
        shape.mesh.indices.push_back(start + columns);
        shape.mesh.indices.push_back(start);
      }
    }
    _init_gfx_resources(&shape);
    return shape;
  }

  Shape CreateSphere(float radius, int subdivisions)
  {
    Shape shape;
    // VERTICES
    auto thetaStep = glm::two_pi<float>() / subdivisions;
    auto phiStep = glm::pi<float>() / subdivisions;
    for (auto row = 0; row <= subdivisions; row++)
    {
      float phi = row * phiStep;
      for (auto col = 0; col <= subdivisions; col++)
      {
        float theta = thetaStep * col;
        shape.mesh.vertices.push_back({
            .position = {cosf(theta) * sinf(phi) * radius, cosf(phi) * radius, sinf(theta) * sinf(phi) * radius},
            .normal = {cosf(theta) * sinf(phi), cosf(phi), sinf(theta) * sinf(phi)},
            .texcoord = {(float)col / subdivisions, 1.0f - ((float)row / subdivisions)},
        });
      }
    }

    // INDICES
    unsigned int columns = subdivisions + 1;
    unsigned int sideStart = columns;
    unsigned int poleStart = 0;
    // Top cap
    for (auto i = 0; i < subdivisions; i++)
    {
      shape.mesh.indices.push_back(sideStart + i);
      shape.mesh.indices.push_back(poleStart + i);
      shape.mesh.indices.push_back(sideStart + i + 1);
    }
    // Rows of quads for sides
    for (auto row = 1; row < subdivisions - 1; row++)
    {
      for (auto col = 0; col < subdivisions; col++)
      {
        unsigned int start = row * columns + col;
        shape.mesh.indices.push_back(start);
        shape.mesh.indices.push_back(start + 1);
        shape.mesh.indices.push_back(start + columns);
        shape.mesh.indices.push_back(start + columns);
        shape.mesh.indices.push_back(start + 1);
        shape.mesh.indices.push_back(start + columns + 1);
      }
    }
    // Bottom cap
    poleStart = (columns * columns) - columns;
    sideStart = poleStart - columns;
    for (size_t i = 0; i < subdivisions; i++)
    {
      shape.mesh.indices.push_back(sideStart + i);
      shape.mesh.indices.push_back(sideStart + i + 1);
      shape.mesh.indices.push_back(poleStart + i);
    }
    _init_gfx_resources(&shape);
    return shape;
  }
}