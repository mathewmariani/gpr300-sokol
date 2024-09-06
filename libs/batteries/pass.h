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

  public:
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
  };
}