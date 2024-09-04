#include "postprocess.h"

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
    PostProcess::PostProcess()
    {
        auto shader = (sg_shader_desc){
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
            .shader = sg_make_shader(shader),
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .label = "postprocess-pipeline",
        });
    }
}