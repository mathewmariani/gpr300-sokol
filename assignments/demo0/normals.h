#pragma once

// batteries
#include "batteries/pass.h"

// glm
#include "glm/mat4x4.hpp"

// shader
#include "normals.glsl.h"

struct Normals final : public batteries::Pass
{
    struct vs_params_t
    {
        glm::mat4 view_proj;
        glm::mat4 model;
        bool normals;
    };

    Normals()
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
                    .source = normals_vs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
                        .uniforms = {
                            [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                            [2] = {.name = "normals", .type = SG_UNIFORMTYPE_INT},
                        },
                    },
                },
                .fs = {
                    .source = normals_fs,

                },
            }),
            .index_type = SG_INDEXTYPE_UINT16,
            .face_winding = SG_FACEWINDING_CCW,
            .cull_mode = SG_CULLMODE_BACK,
            .colors = {
                [0].pixel_format = SG_PIXELFORMAT_RGBA8,
            },
            .depth = {
                .pixel_format = SG_PIXELFORMAT_DEPTH,
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = true,
            },
            .label = "normals-pipeline",
        });
    }
};