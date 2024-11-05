// glm
#include <glm/vec4.hpp>

#include <random>
#include <cmath>

namespace batteries
{
  glm::vec4 random_point_on_sphere()
  {
    // Random number generation with a uniform distribution
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    auto u = dist(gen);
    auto v = dist(gen);

    auto phi = 2.0f * M_PI * u;
    auto theta = acos(2.0f * v - 1.0f);

    auto x = sin(theta) * cos(phi);
    auto y = sin(theta) * sin(phi);
    auto z = cos(theta);

    return {x, y, z, 1.0f};
  }
}
