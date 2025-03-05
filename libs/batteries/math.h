// glm
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace batteries
{
  glm::vec4 random_color(void);
  glm::vec4 random_point_on_sphere(void);
  glm::mat4 random_model_matrix(glm::vec3 position);
}
