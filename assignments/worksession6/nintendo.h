#pragma once

// batteries
#include "batteries/model.h"
#include "batteries/texture.h"

// glm
#include "glm/glm.hpp"

namespace nintendo
{
  struct Palette
  {
    glm::vec3 highlight;
    glm::vec3 shadow;
  };

  struct Model : public batteries::Model
  {
    std::vector<batteries::Texture> textures;
    void Load(const std::string &path);
  };
}