#pragma once

// glm
#include "glm/glm.hpp"

// batteries
#include "pass.h"
#include "shape.h"
#include "shapes.glsl.h"

// sokol
#include "sokol/sokol_gfx.h"

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

        sg_bindings bindings;
        batteries::shape_t sphere;

        Gizmo()
        {
            pipeline = sg_make_pipeline({
                .shader = sg_make_shader({
                    .vs = {
                        .source = shapes_vs,
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
                        .source = shapes_fs,
                        .uniform_blocks[0] = {
                            .layout = SG_UNIFORMLAYOUT_NATIVE,
                            .size = sizeof(fs_params_t),
                            .uniforms = {
                                [0] = {.name = "color", .type = SG_UNIFORMTYPE_FLOAT3},
                            },
                        },
                    },
                }),
                .layout = {
                    .buffers[0] = sshape_vertex_buffer_layout_state(),
                    .attrs = {
                        [0] = sshape_position_vertex_attr_state(),
                        [1] = sshape_normal_vertex_attr_state(),
                        [2] = sshape_texcoord_vertex_attr_state(),
                        [3] = sshape_color_vertex_attr_state(),
                    },
                },
                .index_type = SG_INDEXTYPE_UINT16,
                .cull_mode = SG_CULLMODE_NONE,
                .depth = {
                    .pixel_format = SG_PIXELFORMAT_DEPTH,
                    .compare = SG_COMPAREFUNC_LESS_EQUAL,
                    .write_enabled = true,
                },
                .label = "gizmo-pipeline",
            });

            sphere = batteries::BuildSphere(0.125f, 5, 4);
            bindings = (sg_bindings){
                .vertex_buffers[0] = sphere.vertex_buffer,
                .index_buffer = sphere.index_buffer,
            };
        }
    };
}