#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  // FIXME: could be a struct, and a single header
  class PostProcess
  {
  public:
    PostProcess();
    ~PostProcess() = default;

  protected:
    sg_pipeline pip;
  };
}