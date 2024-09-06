#pragma once

// batteries
#include "batteries/postprocess.h"
#include "batteries/shaders/chromatic_aberration.h"

struct ChromaticAberration : public batteries::PostProcess
{
  ChromaticAberration()
  {
    pip = sg_make_pipeline({
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT2,
                [1].format = SG_VERTEXFORMAT_FLOAT2,
            },
        },
        .shader = sg_make_shader({
            .vs = {
                .source = chromatic_aberration_vs,
            },
            .fs = {
                .source = chromatic_aberration_fs,
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