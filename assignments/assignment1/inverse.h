#pragma once

// batteries
#include "batteries/postprocess.h"
#include "batteries/shaders/inverse_post_process.h"

struct Inverse final : public batteries::PostProcessEffectSettings
{
    struct fs_params_t
    {
    } fs_params;
};

struct InverseRenderer final : public batteries::PostProcessEffect<Inverse>
{
    InverseRenderer()
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
                    .source = inverse_post_process_vs,
                },
                .fs = {
                    .source = inverse_post_process_fs,
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
            .index_type = SG_INDEXTYPE_NONE,
            .face_winding = SG_FACEWINDING_CCW,
            .cull_mode = SG_CULLMODE_BACK,
            .depth = {
                .pixel_format = SG_PIXELFORMAT_DEPTH,
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = true,
            },
            .label = "inverse-pipeline",
        });
    }

    void Render(void)
    {
        sg_apply_pipeline(pipeline);
        sg_apply_bindings(&bindings);
        // sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(settings.fs_params));
        sg_draw(0, 6, 1);
    }
};