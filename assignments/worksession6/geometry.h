#pragma once

#include "nintendo.h"

// batteries
#include "batteries/pass.h"

// glm
#include "glm/mat4x4.hpp"

// shader
#include "geometry.glsl.h"

struct GeometryPass final : public batteries::Pass
{
    typedef struct
    {
        glm::mat4 view_proj;
        glm::mat4 model;
    } vs_params_t;

    typedef struct
    {
        nintendo::Palette palette;
    } fs_params_t;

    GeometryPass()
    {
        pipeline = sg_make_pipeline({
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT3,
                    [1].format = SG_VERTEXFORMAT_FLOAT3,
                    [2].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader({
                .vs = {
                    .source = geometry_vs,
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
                    .source = geometry_fs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(fs_params_t),
                        .uniforms = {
                            [0] = {.name = "palette.highlight", .type = SG_UNIFORMTYPE_FLOAT3},
                            [1] = {.name = "palette.shadow", .type = SG_UNIFORMTYPE_FLOAT3},
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
            .index_type = SG_INDEXTYPE_UINT16,
            .face_winding = SG_FACEWINDING_CCW,
            .cull_mode = SG_CULLMODE_BACK,
            .depth = {
                .pixel_format = SG_PIXELFORMAT_DEPTH,
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = true,
            },
            .label = "windwaker-geometry-pipeline",
        });
    }
};