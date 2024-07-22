#pragma once

#include "glm/glm.hpp"

namespace batteries
{
  struct ambient_t
  {
    float intensity;
    glm::vec3 color;
    glm::vec3 direction;
  };
}