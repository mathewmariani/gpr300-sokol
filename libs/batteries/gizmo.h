#pragma once

// glm
#include "glm/glm.hpp"

// batteries
#include "pass.h"
#include "gizmo.glsl.h"

namespace batteries
{
    struct Gizmo final : public Pass
    {
        struct vs_params_t
        {
            glm::mat4 view_proj;
            glm::mat4 model;
        };

        struct fs_params_t
        {
            glm::vec3 color;
        };

        Gizmo()
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
                        .source = gizmo_vs,
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
                        .source = gizmo_fs,
                        .uniform_blocks[0] = {
                            .layout = SG_UNIFORMLAYOUT_NATIVE,
                            .size = sizeof(fs_params_t),
                            .uniforms = {
                                [0] = {.name = "color", .type = SG_UNIFORMTYPE_FLOAT3},
                            },
                        },
                    },
                }),
                .index_type = SG_INDEXTYPE_UINT16,
                .cull_mode = SG_CULLMODE_NONE,
                .depth = {
                    .pixel_format = SG_PIXELFORMAT_DEPTH,
                    .compare = SG_COMPAREFUNC_LESS_EQUAL,
                    .write_enabled = true,
                },
                .label = "gizmo-pipeline",
            });
        }
    };
}