#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/pass.h"

struct ToonShading final : public batteries::Pass
{
  struct Palette
  {
    glm::vec3 highlight;
    glm::vec3 shadow;
  };

  struct vs_params_t
  {
    glm::mat4 view_proj;
    glm::mat4 model;
  };

  struct fs_params_t
  {
    batteries::light_t light;
    Palette palette;
  };

  ToonShading();
  void Render(const vs_params_t vs_params, const fs_params_t fs_params, batteries::model_t model);
};