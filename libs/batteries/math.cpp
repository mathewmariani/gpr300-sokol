// batteries
#include "math.h"

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include "glm/gtc/random.hpp"

// std
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

    glm::mat4 random_model_matrix(glm::vec3 position)
    {
        auto pitch = glm::linearRand(-glm::pi<float>(), glm::pi<float>());
        auto yaw = glm::linearRand(-glm::pi<float>(), glm::pi<float>());
        auto roll = glm::linearRand(-glm::pi<float>(), glm::pi<float>());

        auto rotationPitch = glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        auto rotationYaw = glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        auto rotationRoll = glm::rotate(glm::mat4(1.0f), roll, glm::vec3(0.0f, 0.0f, 1.0f));

        auto rotation = rotationPitch * rotationYaw * rotationRoll;
        auto model = glm::translate(glm::mat4(1.0f), position);

        return model * rotation;
    }
}