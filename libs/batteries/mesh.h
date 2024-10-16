#pragma once

// sokol
#include "sokol/sokol_gfx.h"

// std
#include <vector>

namespace batteries
{
  struct Mesh
  {
    // sokol info
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;

    std::vector<float> vertices;
    std::vector<uint16_t> indices;
    int num_faces = 0;
  };
}