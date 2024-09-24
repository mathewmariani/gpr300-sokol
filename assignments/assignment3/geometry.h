#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/pass.h"

struct Geometry final : public batteries::Pass
{
  struct vs_params_t
  {
    glm::mat4 view_proj;
  };

  Geometry();
  void Render(const vs_params_t vs_params, batteries::model_t model, int num_instances);
};