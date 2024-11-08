#pragma once

// batteries
#include "batteries/postprocess.h"
#include "transition.glsl.h"

// glm
#include "glm/vec3.hpp"

struct Transition final : public batteries::PostProcessEffectSettings
{
    struct fs_params_t
    {
        float cutoff;
        glm::vec3 color;
    } fs_params;
};

struct TransitionRenderer final : public batteries::PostProcessEffect<Transition>
{
    TransitionRenderer()
    {
        pipeline = sg_make_pipeline((sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader((sg_shader_desc){
                .vertex_func.source = transition_vs,
                .fragment_func.source = transition_fs,
                .uniform_blocks[0] = {
                    .stage = SG_SHADERSTAGE_FRAGMENT,
                    .layout = SG_UNIFORMLAYOUT_NATIVE,
                    .size = sizeof(Transition::fs_params_t),
                    .glsl_uniforms = {
                        [0] = {.glsl_name = "transition.cutoff", .type = SG_UNIFORMTYPE_FLOAT},
                        [1] = {.glsl_name = "transition.color", .type = SG_UNIFORMTYPE_FLOAT3},
                    },
                },
                .images[0].stage = SG_SHADERSTAGE_FRAGMENT,
                .samplers[0].stage = SG_SHADERSTAGE_FRAGMENT,
                .image_sampler_pairs[0] = {
                    .stage = SG_SHADERSTAGE_FRAGMENT,
                    .glsl_name = "transition.gradient",
                    .image_slot = 0,
                    .sampler_slot = 0,
                },
            }),
            .label = "transition-pipeline",
        });
    }
};