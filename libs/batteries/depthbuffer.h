#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  struct Depthbuffer
  {
    sg_pass pass;
    sg_attachments attachments;
    sg_image depth;

    Depthbuffer();
  };
}