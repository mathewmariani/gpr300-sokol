#pragma once

namespace glm
{
  class vec3;
  class vec2;
}

namespace batteries
{
  struct ambient_t
  {
    float intensity;
    glm::vec3 color;
    glm::vec3 direction;
  };
}