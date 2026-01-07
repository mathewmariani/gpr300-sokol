#pragma once

// glm
#include "glm/vec3.hpp"

namespace batteries
{
  struct material_t
  {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
  };
}