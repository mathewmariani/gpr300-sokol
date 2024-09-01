#pragma once

// glm
#include "glm/glm.hpp"

// batteries
#include "lights.h"
#include "materials.h"
#include "geometry.h"

// sokol
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_shape.h"

#define MAX_LIGHTS 64

namespace batteries
{
  struct vs_skybox_params_t
  {
    glm::mat4 view_proj;
  };

  struct skybox_t
  {
    sg_pipeline pip;
    sg_bindings bind;
  };

  void create_skybox_pass(skybox_t *skybox);
}