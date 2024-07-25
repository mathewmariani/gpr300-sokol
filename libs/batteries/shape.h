#pragma once

// sokol
#include "sokol/sokol_shape.h"

// batteries
#include "transform.h"

namespace batteries
{
  struct shape_t
  {
    transform_t transform;
    sshape_element_range_t draw;
  };
}