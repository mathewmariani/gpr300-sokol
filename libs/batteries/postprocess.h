#pragma once

// sokol
#include "sokol/sokol_gfx.h"

// stl
#include <type_traits>

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
    static_assert(std::is_same<T, PostProcessEffectSettings>::value, "Template parameter T must be PostProcessEffectSettings");

    sg_pass pass;
    sg_pipeline pipeline;
    T settings;
  };
}