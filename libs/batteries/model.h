#pragma once

// batteries
#include "transform.h"
#include "vertex.h"
#include "mesh.h"

namespace batteries
{
  struct model_t
  {
    mesh_t mesh;
    transform_t transform;

    void Render(void)
    {
      sg_apply_bindings(&mesh.bindings);
      sg_draw(0, mesh.num_faces * 3, 1);
    };
  };
}