#pragma once

// sokol
#include "sokol/sokol_gfx.h"

// stl
#include <type_traits>

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
  // FIXME: could be a struct, and a single header
  class PostProcess
  {
  public:
    PostProcess();
    ~PostProcess() = default;

    sg_pipeline pip;
  };

  struct PostProcessEffectSettings
  {
    bool active = true;
  };

  template <typename T>
  struct PostProcessEffect
  {
    // static_assert(std::is_same<T, PostProcessEffectSettings>::value, "Template parameter T must be PostProcessEffectSettings");

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
  };
}