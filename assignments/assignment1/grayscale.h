#pragma once

// batteries
#include "batteries/postprocess.h"

#include "grayscale_post_process.glsl.h"

struct GrayScale final : public batteries::PostProcessEffectSettings
{
    struct fs_params_t
    {
    } fs_params;
};

struct GrayScaleRenderer final : public batteries::PostProcessEffect<GrayScale>
{
    GrayScaleRenderer()
    {
        pipeline = sg_make_pipeline({
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader({
                .vs = {
                    .source = grayscale_post_process_vs,
                },
                .fs = {
                    .source = grayscale_post_process_fs,
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
            .label = "grayscale-pipeline",
        });
    }
};