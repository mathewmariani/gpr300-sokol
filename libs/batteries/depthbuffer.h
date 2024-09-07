#pragma once

// sokol
#include "sokol/sokol_gfx.h"

static constexpr int depth_map_size = 1024;

namespace batteries
{
  struct Depthbuffer
  {
    sg_pass pass;
    sg_image depth;

    Depthbuffer()
    {
      depth = sg_make_image({
          .render_target = true,
          .width = depth_map_size,
          .height = depth_map_size,
          .sample_count = 1,
          .pixel_format = SG_PIXELFORMAT_DEPTH,
          .label = "depthbuffer-image",
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