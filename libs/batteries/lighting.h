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
  struct my_light_t
  {
    glm::vec4 color[MAX_LIGHTS];
    glm::vec4 position[MAX_LIGHTS];
  };

  struct fs_lighting_params_t
  {
    glm::vec3 camera_position;
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