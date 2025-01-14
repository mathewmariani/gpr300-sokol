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
        pipeline = sg_make_pipeline((sg_pipeline_desc){
            .layout = {
                // need to provide vertex stride, because normal and texcoords components are skipped in shadow pass
                .buffers[0].stride = 8 * sizeof(float),
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT3,
                },
            },
            .shader = sg_make_shader((sg_shader_desc){
                .vertex_func.source = depth_vs,
                .fragment_func.source = depth_fs,
                .uniform_blocks[0] = {
                    .stage = SG_SHADERSTAGE_VERTEX,
                    .layout = SG_UNIFORMLAYOUT_NATIVE,
                    .size = sizeof(vs_params_t),
                    .glsl_uniforms = {
                        [0] = {.glsl_name = "model", .type = SG_UNIFORMTYPE_MAT4},
                        [1] = {.glsl_name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                    },
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