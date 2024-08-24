#pragma once

// glm
#include "glm/glm.hpp"

// sokol
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_shape.h"

namespace batteries
{
  struct vs_gizmo_params_t
  {
    glm::mat4 view_proj;
    glm::mat4 model;
  };

  struct fs_gizmo_light_params_t
  {
    glm::vec3 light_color;
  };

  struct gizmo_t
  {
    sg_pass_action action;
    sg_pipeline pip;
    sg_bindings bind;
    sshape_element_range_t sphere;
  };

  void create_gizmo_pass(gizmo_t *pass);
}