#pragma once

// sokol
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_shape.h"

// batteries
#include "transform.h"

namespace batteries
{
  struct shape_t
  {
    Transform transform;
    sshape_element_range_t draw;
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
  };
}