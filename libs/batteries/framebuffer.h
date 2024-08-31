#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  struct framebuffer_t
  {
    sg_attachments attachments;
    sg_image color;
    sg_image depth;
    sg_sampler sampler;

    sg_pass_action action;
    sg_pipeline pip;
    sg_bindings bind;
  };

  void create_framebuffer(framebuffer_t *framebuffer, int width, int height);
}