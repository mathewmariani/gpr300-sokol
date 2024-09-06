#pragma once

// sokol
#include "sokol/sokol_gfx.h"

// batteries
#include "batteries/postprocess.h"

namespace batteries
{
  struct Framebuffer
  {
    sg_attachments attachments;
    sg_image color;
    sg_image depth;

    sg_pass pass;
    sg_pass_action action;
    sg_pipeline pip;
    sg_bindings bind;

    Framebuffer();

    void Render();
    void Render(const PostProcess *effect);
  };
}