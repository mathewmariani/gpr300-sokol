#pragma once

// glm
#include "glm/glm.hpp"

// batteries
#include "batteries/model.h"
#include "batteries/pass.h"

struct Depth final : public batteries::Pass
{
  struct vs_params_t
  {
    glm::mat4 view_proj;
    glm::mat4 model;
  };

  Depth();
  void Render(const vs_params_t vs_params, batteries::model_t model);
};