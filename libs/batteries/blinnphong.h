#pragma once

// batteries
#include "lights.h"
#include "materials.h"

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  struct vs_blinnphong_params_t
  {
    glm::mat4 view_proj;
    glm::mat4 model;
  };

  struct fs_blinnphong_params_t
  {
    material_t material;
    light_t light;
    glm::vec3 camera_position;
  };

  struct blinnphong_t
  {
    sg_pass_action action;
    sg_pipeline pip;
    sg_bindings bind;
  };

  void create_blinnphong_pass(blinnphong_t *pass);
}