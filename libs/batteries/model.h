#pragma once

// batteries
#include "assets.h"
#include "transform.h"
#include "mesh.h"

namespace batteries
{
  struct Model : public batteries::Asset
  {
    Mesh mesh;
    Transform transform;

    Model() = default;

    virtual void Load(const std::string &path);
  };
}