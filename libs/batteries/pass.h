#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  class Pass
  {
  public:
    Pass() = default;
    ~Pass() = default;

  protected:
    sg_pass_action action;
    sg_pipeline pip;
    sg_bindings bind;
  };
}