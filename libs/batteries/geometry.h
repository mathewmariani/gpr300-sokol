#pragma once

// batteries
#include "lights.h"
#include "materials.h"
#include "vertex.h"

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  struct vs_geometry_params_t
  {
    glm::mat4 view_proj;
  };

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

  struct geometry_t
  {
    sg_pass_action action;
    sg_attachments attachments;
    sg_pipeline pip;
    sg_bindings bind;

    sg_bindings img;
    sg_image color_img;
    sg_image position_img;
    sg_image normal_img;
    sg_image depth_img;
  };

  void create_geometry_pass(geometry_t *pass, int width, int height);
}