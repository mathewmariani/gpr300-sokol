#pragma once

// sokol
#include "sokol/sokol_gfx.h"

// batteries
#include "batteries/postprocess.h"

#include <functional>

namespace batteries
{
  struct Framebuffer
  {
    sg_pass pass;
    sg_pipeline pipeline;
    sg_bindings bindings;
    sg_image color;
    sg_image depth;

    Framebuffer(void);
  };
}