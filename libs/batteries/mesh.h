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
    sg_pipeline pipeline;
    sg_bindings bindings;

    std::vector<float> vertices;
    std::vector<uint16_t> indices;
    int num_faces = 0;

    Mesh() = default;
    void Render(void)
    {
      if (num_faces <= 0)
      {
        return;
      }
      sg_apply_bindings(&bindings);
      sg_draw(0, num_faces * 3, 1);
    }
  };
}