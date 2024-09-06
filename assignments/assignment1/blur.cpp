#include "blur.h"

#include "batteries/shaders/blur_post_process.h"

Blur::Blur()
{
    pass_action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        },
    };

    pip = sg_make_pipeline({
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT2,
                [1].format = SG_VERTEXFORMAT_FLOAT2,
            },
        },
        .shader = sg_make_shader({
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
        }),
        .label = "blur-pipeline",
    });
}