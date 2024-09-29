#include "framebuffer.h"
#include "sokol/sokol_app.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_gfx.h"

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
    Framebuffer::Framebuffer(void)
        : effect{nullptr}
    {
        sg_image_desc img_desc = {
            .render_target = true,
            .width = 800,
            .height = 600,
        };

        // color attachment
        img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        img_desc.label = "framebuffer-color-image";
        color = sg_make_image(img_desc);

        // depth attachment
        img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
        img_desc.label = "framebuffer-depth-image";
        depth = sg_make_image(img_desc);

        attachments = sg_make_attachments({
            .colors[0].image = color,
            .depth_stencil.image = depth,
            .label = "framebuffer-attachments",
        });

        pass = (sg_pass){
            .action = (sg_pass_action){
                .colors[0] = {
                    .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
                    .load_action = SG_LOADACTION_CLEAR,
                },
            },
            .swapchain = sglue_swapchain(),
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

        pip = sg_make_pipeline({
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
        bind = (sg_bindings){
            .vertex_buffers[0] = sg_make_buffer({
                .data = SG_RANGE(quad_vertices),
                .label = "quad-vertices",
            }),
            .fs = {
                .images[0] = color,
                .samplers[0] = sg_make_sampler({
                    .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
                    .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
                    .min_filter = SG_FILTER_LINEAR,
                    .mag_filter = SG_FILTER_LINEAR,
                }),
            },
        };
    }

    void Framebuffer::Render(void)
    {
        sg_begin_pass(&pass);
        if (effect)
        {
            effect->Apply();
        }
        else
        {
            sg_apply_pipeline(pip);
            sg_apply_bindings(&bind);
        }
        sg_draw(0, 6, 1);
    }

    void Framebuffer::ApplyEffect(BasePostProcessEffect *effect)
    {
        this->effect = effect;
    }
}