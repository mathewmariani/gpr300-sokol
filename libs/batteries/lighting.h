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
  struct fs_lighting_params_t
  {
    glm::vec3 eye;
    batteries::ambient_t ambient;
    batteries::material_t material;
    batteries::pointlight_t lights[64];
    struct
    {
      float radius;
    } imgui;
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