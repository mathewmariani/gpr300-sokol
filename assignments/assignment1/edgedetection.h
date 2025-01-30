#pragma once

// batteries
#include "batteries/postprocess.h"

#include "edgedetection.glsl.h"

struct EdgeDetection final : public batteries::PostProcessEffectSettings
{
    struct fs_params_t
    {
    } fs_params;
};

struct EdgeDetectionRenderer final : public batteries::PostProcessEffect<EdgeDetection>
{
    EdgeDetectionRenderer()
    {
        pipeline = sg_make_pipeline((sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader((sg_shader_desc){
                .vertex_func.source = edgedetection_vs,
                .fragment_func.source = edgedetection_fs,
                .images[0].stage = SG_SHADERSTAGE_FRAGMENT,
                .samplers[0].stage = SG_SHADERSTAGE_FRAGMENT,
                .image_sampler_pairs[0] = {
                    .stage = SG_SHADERSTAGE_FRAGMENT,
                    .glsl_name = "screen",
                    .image_slot = 0,
                    .sampler_slot = 0,
                },
            }),
            .label = "transition-pipeline",
        });
    }
};