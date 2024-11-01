#pragma once

// glm
#define GLM_FORCE_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace batteries
{
  struct Transform
  {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::mat4 matrix() const
    {
      glm::mat4 m = glm::mat4(1.0f);
      m = glm::translate(m, position);
      m *= glm::mat4_cast(rotation);
      m = glm::scale(m, scale);
      return m;
    }
  };
}