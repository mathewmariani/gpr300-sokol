#pragma once

// batteries
#include "batteries/assets.h"
#include "batteries/model.h"
#include "batteries/texture.h"

namespace nintendo
{
  struct Model : public batteries::Model
  {
    batteries::Texture albedo;
    batteries::Texture zatoon;

    void Load(const std::string &path);
  };
}