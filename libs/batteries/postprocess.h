#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  struct PostProcessEffectSettings
  {
    bool active = true;
  };

  struct BasePostProcessEffect
  {
    virtual ~BasePostProcessEffect() = default;
  };

  template <typename T>
  struct PostProcessEffect : public BasePostProcessEffect
  {
    sg_pipeline pipeline;
    T settings;
  };
}