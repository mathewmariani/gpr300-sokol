#pragma once

// batteries
#include "batteries/postprocess.h"

#include "bloom.glsl.h"

struct Bloom final : public batteries::PostProcessEffectSettings
{
    struct fs_params_t
    {
    } fs_params;
};

struct BloomRenderer final : public batteries::PostProcessEffect<Bloom>
{
    BloomRenderer()
    {
        pipeline = sg_make_pipeline((sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader((sg_shader_desc){
                .vertex_func.source = bloom_vs,
                .fragment_func.source = bloom_fs,
                .images[0].stage = SG_SHADERSTAGE_FRAGMENT,
                .samplers[0].stage = SG_SHADERSTAGE_FRAGMENT,
                .image_sampler_pairs[0] = {
                    .stage = SG_SHADERSTAGE_FRAGMENT,
                    .glsl_name = "screen",
                    .image_slot = 0,
                    .sampler_slot = 0,
                },
            }),
            .sample_count = 1,
            .color_count = 3,
            .colors = {
                [0].pixel_format = SG_PIXELFORMAT_RGBA16F,
                [1].pixel_format = SG_PIXELFORMAT_RGBA16F,
                [2].pixel_format = SG_PIXELFORMAT_RGBA8,
            },
            .label = "bloom-pipeline",
        });
    }
};