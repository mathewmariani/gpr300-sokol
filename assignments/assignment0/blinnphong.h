#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"

// sokol
#include "sokol/sokol_gfx.h"

struct vs_blinnphong_params_t
{
  glm::mat4 view_proj;
  glm::mat4 model;
};

struct fs_blinnphong_params_t
{
  batteries::material_t material;
  batteries::light_t light;
  batteries::ambient_t ambient;
  glm::vec3 camera_position;
};

struct blinnphong_t
{
  sg_pass_action action;
  sg_pipeline pip;
};

void create_blinnphong_pass(blinnphong_t *pass);