#pragma once

// batteries
#include "transform.h"
#include "vertex.h"

#include <vector>

namespace batteries
{
  struct model_t
  {
    mesh_t mesh;
    transform_t transform;
  };
}