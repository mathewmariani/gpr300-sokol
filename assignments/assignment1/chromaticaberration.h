#pragma once

// batteries
#include "batteries/postprocess.h"

#include "chromaticaberration.glsl.h"

struct ChromaticAberration final : public batteries::PostProcessEffectSettings
{
    struct fs_params_t
    {
    } fs_params;
};

struct ChromaticAberrationRenderer final : public batteries::PostProcessEffect<ChromaticAberration>
{
    ChromaticAberrationRenderer()
    {
        pipeline = sg_make_pipeline((sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader((sg_shader_desc){
                .vertex_func.source = chromaticaberration_vs,
                .fragment_func.source = chromaticaberration_fs,
                .images[0].stage = SG_SHADERSTAGE_FRAGMENT,
                .samplers[0].stage = SG_SHADERSTAGE_FRAGMENT,
                .image_sampler_pairs[0] = {
                    .stage = SG_SHADERSTAGE_FRAGMENT,
                    .glsl_name = "screen",
                    .image_slot = 0,
                    .sampler_slot = 0,
                },
            }),
            .label = "chromaticaberration-pipeline",
        });
    }
};