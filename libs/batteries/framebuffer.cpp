#include "framebuffer.h"

namespace
{
    // clang-format off
  const char no_post_process_vs[] = R"(#version 300 es
      // attributes
      layout(location = 0) in vec2 in_position;
      layout(location = 1) in vec2 in_texcoord;
      out vec2 texcoords;
      void main()
      {
        texcoords = in_texcoord;
        gl_Position = vec4(in_position.xy, 0.0, 1.0);
      })";
  const char no_post_process_fs[] = R"(#version 300 es
      precision mediump float;
      out vec4 FragColor;
      in vec2 texcoords;
      uniform sampler2D screen;
      void main()
      {
        FragColor = vec4(texture(screen, texcoords).rgb, 1.0);
      })";
    // clang-format on
}

namespace batteries
{
    void create_framebuffer(framebuffer_t *framebuffer, int width, int height)
    {
        // color attachment
        framebuffer->color = sg_make_image({
            .pixel_format = SG_PIXELFORMAT_RGBA8,
            .render_target = true,
            .width = width,
            .height = height,
            .label = "framebuffer-color-image",
        });

        // depth attachment
        framebuffer->depth = sg_make_image({
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .render_target = true,
            .width = width,
            .height = height,
            .label = "framebuffer-depth-image",
        });

        // image sampler
        framebuffer->sampler = sg_make_sampler({
            .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
            .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
        });

        framebuffer->action = (sg_pass_action){
            .colors[0] = {
                .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
                .load_action = SG_LOADACTION_CLEAR,
            },
        };

        framebuffer->pass = (sg_pass){
            .action = framebuffer->action,
            .attachments = sg_make_attachments({
                .colors[0].image = framebuffer->color,
                .depth_stencil.image = framebuffer->depth,
                .label = "framebuffer-attachments",
            }),
        };

        auto display_shader_desc = (sg_shader_desc){
            .vs = {
                .source = no_post_process_vs,
            },
            .fs = {
                .source = no_post_process_fs,
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

        framebuffer->pip = sg_make_pipeline({
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader(display_shader_desc),
            .label = "display-pipeline",
        });

        // clang-format off
        float quad_vertices[] = {
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
        };
        // clang-format on

        // apply bindings
        framebuffer->bind = (sg_bindings){
            .vertex_buffers[0] = sg_make_buffer({
                .data = SG_RANGE(quad_vertices),
                .label = "quad-vertices",
            }),
            .fs = {
                .images[0] = framebuffer->color,
                .samplers[0] = framebuffer->sampler,
            },
        };
    }
}