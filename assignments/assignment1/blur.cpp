#include "blur.h"

#include "batteries/shaders/blur_post_process.h"

Blur::Blur()
{
  // FIXME: seems unecessary. Just send the shader as a constructor argument. Would be easier?
  auto shader = (sg_shader_desc){
      .vs = {
          .source = blur_post_process_vs,
      },
      .fs = {
          .source = blur_post_process_fs,
          .images[0].used = true,
          .samplers[0].used = true,
          .image_sampler_pairs[0] = {
              .glsl_name = "screen",
              .image_slot = 0,
              .sampler_slot = 0,
              .used = true,
          },
      },
  };

  pip = sg_make_pipeline({
      .layout = {
          .attrs = {
              [0].format = SG_VERTEXFORMAT_FLOAT2,
              [1].format = SG_VERTEXFORMAT_FLOAT2,
          },
      },
      .shader = sg_make_shader(shader),
      .label = "blur-pipeline",
  });
}