#pragma once

// batteries
#include "batteries/postprocess.h"

#include "pixelation.glsl.h"

struct Pixelation final : public batteries::PostProcessEffectSettings
{
    struct fs_params_t
    {
    } fs_params;
};

struct PixelationRenderer final : public batteries::PostProcessEffect<Pixelation>
{
    PixelationRenderer()
    {
        pipeline = sg_make_pipeline((sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader((sg_shader_desc){
                .vertex_func.source = pixelation_vs,
                .fragment_func.source = pixelation_fs,
                .images[0].stage = SG_SHADERSTAGE_FRAGMENT,
                .samplers[0].stage = SG_SHADERSTAGE_FRAGMENT,
                .image_sampler_pairs[0] = {
                    .stage = SG_SHADERSTAGE_FRAGMENT,
                    .glsl_name = "screen",
                    .image_slot = 0,
                    .sampler_slot = 0,
                },
            }),
            .label = "pixelation-pipeline",
        });
    }
};