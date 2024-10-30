// batteries
#include "shape.h"
#include "vertex.h"

namespace batteries
{
  Shape CreatePlane(float width, float height, int subdivisions)
  {
    // VERTICES
    Shape shape;
    for (auto row = 0; row <= subdivisions; row++)
    {
      for (auto col = 0; col <= subdivisions; col++)
      {
        vertex_t v;
        // v.uv.x = ((float)col / subdivisions);
        // v.uv.y = ((float)row / subdivisions);
        // v.pos.x = -width/2 + width * v.uv.x;
        // v.pos.y = 0;
        // v.pos.z = height/2 -height * v.uv.y;
        // v.normal = glm::vec3(0, 1, 0);
        shape.mesh.vertices.push_back(-width / 2 + width * ((float)col / subdivisions));  // p.x
        shape.mesh.vertices.push_back(0);                                                 // p.y
        shape.mesh.vertices.push_back(height / 2 - height * ((float)row / subdivisions)); // p.z
        shape.mesh.vertices.push_back(0.0f);                                              // n.x
        shape.mesh.vertices.push_back(1.0f);                                              // n.y
        shape.mesh.vertices.push_back(0.0f);                                              // n.z
        shape.mesh.vertices.push_back(((float)col / subdivisions));                       // t.x
        shape.mesh.vertices.push_back(((float)row / subdivisions));                       // t.y
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

    // initialize gfx resources
    shape.mesh.vertex_buffer = sg_make_buffer({
        .type = SG_BUFFERTYPE_VERTEXBUFFER,
        .data = {
            .ptr = shape.mesh.vertices.data(),
            .size = shape.mesh.vertices.size() * sizeof(float),
        },
        .label = "mesh-vertices",
    });
    shape.mesh.index_buffer = sg_make_buffer({
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = {
            .ptr = shape.mesh.indices.data(),
            .size = shape.mesh.indices.size() * sizeof(uint16_t),
        },
        .label = "mesh-indices",
    });

    return shape;
  }
}