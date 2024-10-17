#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
  struct Depthbuffer
  {
    sg_pass pass;
    sg_image depth;
    sg_sampler sampler;

    Depthbuffer()
    {
      depth = sg_make_image({
          .render_target = true,
          .width = 1024,
          .height = 1024,
          .sample_count = 1,
          .pixel_format = SG_PIXELFORMAT_DEPTH,
          .label = "depthbuffer-depth",
      });

      sampler = sg_make_sampler({
          .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
          .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
          .min_filter = SG_FILTER_NEAREST,
          .mag_filter = SG_FILTER_NEAREST,
          .compare = SG_COMPAREFUNC_LESS,
          .label = "depthbuffer-sampler",
      });

      pass = (sg_pass){
          .action = (sg_pass_action){
              .depth = {
                  .load_action = SG_LOADACTION_CLEAR,
                  .store_action = SG_STOREACTION_STORE,
                  .clear_value = 1.0f,
              },
          },
          .attachments = sg_make_attachments({
              .depth_stencil.image = depth,
              .label = "depthbuffer-attachments",
          }),
      };
    }
  };
}