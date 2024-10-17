#pragma once

// batteries
#include "batteries/model.h"

namespace assignment2
{
  struct Model : public batteries::Model
  {
    void Load(const std::string &path);
  };
}