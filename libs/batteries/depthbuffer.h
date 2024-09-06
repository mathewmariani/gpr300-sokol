#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  class Depthbuffer
  {
  public:
    Depthbuffer();

  public:
    sg_pass pass;
    sg_attachments attachments;
    sg_image depth;
  };
}