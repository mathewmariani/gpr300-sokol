#pragma once

// batteries
#include "batteries/postprocess.h"

#include "crt.glsl.h"

struct CRT final : public batteries::PostProcessEffectSettings
{
    struct fs_params_t
    {
    } fs_params;
};

struct CRTRenderer final : public batteries::PostProcessEffect<CRT>
{
    CRTRenderer()
    {
        pipeline = sg_make_pipeline({
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader({
                .vertex_func.source = crt_vs,
                .fragment_func.source = crt_vs,
                .images[0].used = true,
                .samplers[0].used = true,
                .image_sampler_pairs[0] = {
                    .glsl_name = "screen",
                    .image_slot = 0,
                    .sampler_slot = 0,
                    .used = true,
                },
            }),
            .label = "crt-pipeline",
        });
    }
};