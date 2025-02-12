#pragma once

// batteries
#include "vertex.h"

// std
#include <vector>

namespace batteries
{
  struct Mesh
  {
    struct Groups
    {
      unsigned int face_count;
      unsigned int face_offset;
      unsigned int index_offset;
    };

    // sokol info
    // sg_buffer vertex_buffer;
    // sg_buffer index_buffer;
    // sg_sampler sampler;

    std::vector<vertex_t> vertices;
    std::vector<uint16_t> indices;
    std::vector<Groups> groups;
    int num_faces = 0;
  };
}