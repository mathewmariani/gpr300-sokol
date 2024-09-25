#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/pass.h"
#include "batteries/shape.h"

struct Water final : public batteries::Pass
{
  struct water_t
  {
    sg_bindings bind;
    sg_image img;
    batteries::shape_t plane;
  };

  struct vs_params_t
  {
    glm::mat4 view_proj;
    glm::mat4 model;
    glm::vec3 camera_pos;
    float scale;
    float strength;
    float tiling;
    float time;
    glm::vec3 color;
    glm::vec2 direction;
    float top_scale;
    float bottom_scale;
  };

  struct fs_params_t
  {
    float scale;
    float strength;
    float tiling;
    float time;
    glm::vec3 color;
    glm::vec2 direction;
    float top_scale;
    float bottom_scale;
  };

  Water();
  void Render(const vs_params_t vs_params, const fs_params_t fs_params, water_t water);
};