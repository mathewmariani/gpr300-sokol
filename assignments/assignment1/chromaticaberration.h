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
        pipeline = sg_make_pipeline({
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader({
                .vs = {
                    .source = chromaticaberration_vs,
                },
                .fs = {
                    .source = chromaticaberration_fs,
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
            .label = "chromatic-aberration-pipeline",
        });
    }
};