#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  struct Pass
  {
    sg_pass_action pass_action;
    sg_pipeline pipeline;
    sg_bindings bindings;

    Pass() = default;
    ~Pass() = default;
  };
}