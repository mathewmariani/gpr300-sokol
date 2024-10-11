#pragma once

// batteries
#include "batteries/model.h"

namespace assignment1
{
  struct Model : public batteries::Model
  {
    void Load(const std::string &path);
  };
}