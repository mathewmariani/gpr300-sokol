#pragma once

// batteries
#include "lights.h"
#include "materials.h"

namespace batteries
{
  struct vs_blinnphong_params_t
  {
    glm::mat4 view_proj;
    glm::mat4 model;
  };

  struct fs_blinnphong_params_t
  {
    material_t material;
    light_t light;
    glm::vec3 camera_position;
  };
}