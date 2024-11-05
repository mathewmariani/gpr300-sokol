#pragma once

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace batteries
{
  struct Transform
  {
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    glm::mat4 matrix() const
    {
      glm::mat4 m{1.0f};
      m = glm::translate(m, position);
      m *= glm::mat4_cast(rotation);
      m = glm::scale(m, scale);
      return m;
    }
  };
}