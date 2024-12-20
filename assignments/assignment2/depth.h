#pragma once

// glm
#include "glm/mat4x4.hpp"

// batteries
#include "batteries/pass.h"

#include "depth.glsl.h"

struct Depth final : public batteries::Pass
{
    struct vs_params_t
    {
        glm::mat4 view_proj;
        glm::mat4 model;
    };

    Depth()
    {
        pipeline = sg_make_pipeline({
            .layout = {
                // need to provide vertex stride, because normal and texcoords components are skipped in shadow pass
                .buffers[0].stride = 8 * sizeof(float),
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT3,
                },
            },
            .shader = sg_make_shader({
                .vs = {
                    .source = depth_vs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
                        .uniforms = {
                            [0] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                            [1] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                        },
                    },
                },
                .fs = {
                    .source = depth_fs,
                },
            }),
            .index_type = SG_INDEXTYPE_UINT16,
            .face_winding = SG_FACEWINDING_CCW,
            .cull_mode = SG_CULLMODE_FRONT,
            .colors[0].pixel_format = SG_PIXELFORMAT_NONE,
            .depth = {
                .pixel_format = SG_PIXELFORMAT_DEPTH,
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = true,
            },
            .label = "depth-pipeline",
        });
    }
};