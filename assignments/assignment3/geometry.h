#pragma once

// batteries
#include "batteries/pass.h"
#include "batteries/vertex.h"

#include "geometry.glsl.h"

struct Geometry final : public batteries::Pass
{
    struct vs_params_t
    {
        glm::mat4 view_proj;
    };

    Geometry()
    {
        pipeline = sg_make_pipeline({
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT3,
                    [1].format = SG_VERTEXFORMAT_FLOAT3,
                    [2].format = SG_VERTEXFORMAT_FLOAT2,
                    [3] = {.format = SG_VERTEXFORMAT_FLOAT4, .buffer_index = 1},
                    [4] = {.format = SG_VERTEXFORMAT_FLOAT4, .buffer_index = 1},
                    [5] = {.format = SG_VERTEXFORMAT_FLOAT4, .buffer_index = 1},
                    [6] = {.format = SG_VERTEXFORMAT_FLOAT4, .buffer_index = 1},
                },
                .buffers = {
                    [0].stride = sizeof(batteries::vertex_t),
                    [1] = {
                        .stride = sizeof(glm::mat4),
                        .step_func = SG_VERTEXSTEP_PER_INSTANCE,
                    },
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
                        },
                    },
                },
                .fs = {
                    .source = geometry_fs,
                },
            }),
            .index_type = SG_INDEXTYPE_UINT16,
            .face_winding = SG_FACEWINDING_CCW,
            .cull_mode = SG_CULLMODE_BACK,
            .sample_count = 1,
            .color_count = 3,
            .colors = {
                [0].pixel_format = SG_PIXELFORMAT_RGBA16F,
                [1].pixel_format = SG_PIXELFORMAT_RGBA16F,
                [2].pixel_format = SG_PIXELFORMAT_RGBA8,
            },
            .depth = {
                .pixel_format = SG_PIXELFORMAT_DEPTH,
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = true,
            },
            .label = "geometry-pipeline",
        });
    }
};