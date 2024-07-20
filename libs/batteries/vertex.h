#pragma once

namespace glm
{
  class vec3;
  class vec2;
}

struct vertex_t
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texcoord;
};