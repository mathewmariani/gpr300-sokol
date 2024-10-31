#pragma once

// batteries
#include "batteries/assets.h"
#include "batteries/model.h"
#include "batteries/texture.h"

namespace nintendo
{
  struct Model : public batteries::Model
  {
    batteries::Texture col;
    batteries::Texture mtl;
    batteries::Texture rgh;
    batteries::Texture ao;
    batteries::Texture spc;

    void Load(const std::string &path);
  };
}