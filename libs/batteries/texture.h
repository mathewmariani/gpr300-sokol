#pragma once

// batteries
#include "assets.h"

namespace batteries
{
  struct Texture final : public batteries::Asset
  {
    Texture()
    {
    }

    ~Texture()
    {
      // sg_destroy_image(image);
    }

    void Load(const std::string &path)
    {
      batteries::load_texture({
          .texture = this,
          .path = path.c_str(),
      });
    }
  };
}