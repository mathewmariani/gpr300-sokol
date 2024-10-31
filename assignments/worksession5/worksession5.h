#pragma once

// batteries
#include "batteries/model.h"

namespace worksession5
{
  struct Model : public batteries::Model
  {
    void Load(const std::string &path);
  };
}