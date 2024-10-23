#pragma once

// batteries
#include "assets.h"

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  struct Texture final : public batteries::Asset
  {
    sg_image image;
    void *pixels;

    Texture()
    {
      image = sg_alloc_image();
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