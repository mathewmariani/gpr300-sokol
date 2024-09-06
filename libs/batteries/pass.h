#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  struct Pass
  {
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;

    Pass() = default;
    ~Pass() = default;
  };
}