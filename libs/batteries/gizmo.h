#pragma once

// glm
#include "glm/glm.hpp"

// batteries
#include "pass.h"

// sokol
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_shape.h"

namespace batteries
{
  class Gizmo final : public Pass
  {
  public:
    struct vs_params_t
    {
      glm::mat4 view_proj;
      glm::mat4 model;
    };

    struct fs_params_t
    {
      glm::vec3 color;
    };

  public:
    Gizmo();

    void Render(const vs_params_t vs_params, const fs_params_t fs_params);

  private:
    sshape_element_range_t sphere;
  };
}