#pragma once

// glm
#include "glm/glm.hpp"

// batteries
#include "batteries/model.h"
#include "batteries/pass.h"

class Depth final : public batteries::Pass
{
public:
  struct vs_params_t
  {
    glm::mat4 view_proj;
    glm::mat4 model;
  };

public:
  Depth();

  void Render(const vs_params_t vs_params, batteries::model_t model);
};