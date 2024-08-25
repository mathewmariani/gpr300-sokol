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

namespace batteries
{
  struct my_light_t
  {
    glm::vec4 position[64];
    glm::vec4 color[64];
  };

  struct fs_lighting_params_t
  {
    glm::vec3 eye;
    batteries::my_light_t lights;
  };

  struct lighting_t
  {
    sg_pass_action action;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;
  };

  void create_lighting_pass(lighting_t *pass, geometry_t *geometry);
}