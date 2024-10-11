#pragma once

// batteries
#include "batteries/assets.h"
#include "batteries/model.h"
#include "batteries/materials.h"

namespace assignment0
{
  struct Model : public batteries::Model
  {
    void Load(const std::string &path);
  };
}