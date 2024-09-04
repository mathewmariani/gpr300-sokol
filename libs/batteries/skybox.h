#pragma once

// glm
#include "glm/glm.hpp"

// batteries
#include "pass.h"

namespace batteries
{
  class Skybox final : public Pass
  {
  public:
    struct vs_params_t
    {
      glm::mat4 view_proj;
    };

  public:
    Skybox();

    void Render(const vs_params_t vs_params);
  };
}