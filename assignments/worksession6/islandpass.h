#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/pass.h"

// glm
#include "glm/glm.hpp"

// shader
#include "batteries/shaders/windwaker_object.h"

struct IslandPass final : public batteries::Pass
{
    struct Palette
    {
        glm::vec3 highlight;
        glm::vec3 shadow;
    };

    typedef struct
    {
        glm::mat4 view_proj;
        glm::mat4 model;
    } vs_params_t;

    typedef struct
    {
        batteries::ambient_t ambient;
        Palette palette;
    } fs_params_t;

    IslandPass()
    {
        pip = sg_make_pipeline({
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT3,
                    [1].format = SG_VERTEXFORMAT_FLOAT3,
                    [2].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader({
                .vs = {
                    .source = windwaker_object_vs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
                        .uniforms = {
                            [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                        },
                    },
                },
                .fs = {
                    .source = windwaker_object_fs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(fs_params_t),
                        .uniforms = {
                            [0] = {.name = "ambient.intensity", .type = SG_UNIFORMTYPE_FLOAT},
                            [1] = {.name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [2] = {.name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
                            [3] = {.name = "palette.highlight", .type = SG_UNIFORMTYPE_FLOAT3},
                            [4] = {.name = "palette.shadow", .type = SG_UNIFORMTYPE_FLOAT3},
                        },
                    },
                    .images = {
                        [0] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                    },
                    .samplers = {
                        [0] = {.used = true, .sampler_type = SG_SAMPLERTYPE_FILTERING},
                    },
                    .image_sampler_pairs = {
                        [0] = {
                            .used = true,
                            .glsl_name = "albedo",
                            .image_slot = 0,
                            .sampler_slot = 0,
                        },
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
            .label = "object-pipeline",
        });
    }

    void Apply(const vs_params_t &vs_params, const fs_params_t &fs_params)
    {
        sg_apply_pipeline(pip);
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE_REF(vs_params));
        sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE_REF(fs_params));
    }
};