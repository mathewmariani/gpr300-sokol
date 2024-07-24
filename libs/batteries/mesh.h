#pragma once

// sokol
#include "sokol/sokol_gfx.h"

// fast obj
#include "fast_obj/fast_obj.h"

// batteries
#include "transform.h"

// std
#include <vector>

namespace batteries
{
  // TODO: use vertex_t instead of float
  struct mesh_t
  {
    sg_buffer vbuf;
    sg_buffer ibuf;
    fastObjMesh *obj;
    int num_faces;
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
  };
}