#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/pass.h"

struct Shadow final : public batteries::Pass
{
  struct vs_params_t
  {
    glm::mat4 model;
    glm::mat4 view_proj;
    glm::mat4 light_view_proj;
  };

  struct fs_params_t
  {
    batteries::material_t material;
    batteries::light_t light;
    batteries::ambient_t ambient;
    glm::vec3 camera_position;
  };

  sg_sampler sampler;

  Shadow();
  void Render(const vs_params_t vs_params, const fs_params_t fs_params, sg_image depth, batteries::model_t model);
};