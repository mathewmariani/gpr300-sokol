#pragma once

// batteries
#include "batteries/postprocess.h"
#include "batteries/shaders/transition.h"

// glm
#include "glm/glm.hpp"

// sokol
#include "sokol/sokol_glue.h"

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
        pipeline = sg_make_pipeline({
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader({
                .vs = {
                    .source = transition_vs,
                },
                .fs = {
                    .source = transition_fs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(Transition::fs_params_t),
                        .uniforms = {
                            [0] = {.name = "transition.cutoff", .type = SG_UNIFORMTYPE_FLOAT},
                            [1] = {.name = "transition.color", .type = SG_UNIFORMTYPE_FLOAT3},
                        },
                    },
                    .images[0].used = true,
                    .samplers[0].used = true,
                    .image_sampler_pairs[0] = {
                        .glsl_name = "transition.gradient",
                        .image_slot = 0,
                        .sampler_slot = 0,
                        .used = true,
                    },
                },
            }),
            .label = "transition-pipeline",
        });
    }

    void Render(void)
    {
        if (!settings.active)
        {
            return;
        }

        sg_apply_pipeline(pipeline);
        // sg_apply_bindings(&bindings);
        sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(settings.fs_params));
    }
};