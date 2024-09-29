#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace
{
  // clang-format off
  constexpr float quad_vertices[] = {
     -1.0f, 1.0f, 0.0f, 1.0f,
     -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 1.0f, 0.0f,

     -1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 1.0f, 1.0f,
  };
  // clang-format on
}

namespace batteries
{
  struct PostProcessEffectSettings
  {
    bool active = true;
  };

  struct BasePostProcessEffect
  {
    virtual void Apply() = 0;
    virtual ~BasePostProcessEffect() = default;
  };

  template <typename T>
  struct PostProcessEffect : public BasePostProcessEffect
  {
    sg_pass pass;
    sg_pipeline pipeline;
    sg_bindings bindings;
    T settings;

    PostProcessEffect()
    {
      // apply bindings
      bindings = (sg_bindings){
          .vertex_buffers[0] = sg_make_buffer({
              .data = SG_RANGE(quad_vertices),
              .label = "quad-vertices",
          }),
      };
    }

    virtual void Apply(void) = 0;
  };
}