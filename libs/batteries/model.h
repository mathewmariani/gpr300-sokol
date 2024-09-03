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
  };
}