#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  struct Pass
  {
    sg_pipeline pipeline;
    Pass() = default;
  };
}