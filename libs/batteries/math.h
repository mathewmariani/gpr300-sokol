// glm
#include "glm/glm.hpp"

namespace batteries
{
  static glm::vec4 random_point_on_sphere()
  {
    auto u = static_cast<float>(rand()) / RAND_MAX;
    auto v = static_cast<float>(rand()) / RAND_MAX;

    auto phi = 2.0 * M_PI * u;
    auto theta = acos(2.0f * v - 1.0f);

    auto x = sin(theta) * cos(phi);
    auto y = sin(theta) * sin(phi);
    auto z = cos(theta);

    return glm::vec4(x, y, z, 1.0f);
  }
}