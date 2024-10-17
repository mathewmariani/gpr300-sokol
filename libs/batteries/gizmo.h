#pragma once

// glm
#include "glm/glm.hpp"

// batteries
#include "pass.h"
#include "shaders/shapes.h"

// sokol
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_shape.h"

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
                    .pixel_format = SG_PIXELFORMAT_DEPTH_STENCIL,
                    .compare = SG_COMPAREFUNC_LESS_EQUAL,
                    .write_enabled = true,
                },
                .label = "gizmo-pipeline",
            });

            // generate shape geometries
            sshape_vertex_t vertices[30] = {0}; // (slices + 1) * (stacks + 1);
            uint16_t indices[90] = {0};         // ((2 * slices * stacks) - (2 * slices)) * 3;
            sshape_buffer_t buf = {
                .vertices.buffer = SSHAPE_RANGE(vertices),
                .indices.buffer = SSHAPE_RANGE(indices),
            };
            const sshape_sphere_t shape = {
                .radius = 0.125f,
                .slices = 5,
                .stacks = 4,
            };
            buf = sshape_build_sphere(&buf, &shape);
            assert(buf.valid);

            // one vertex/index-buffer-pair for all shapes
            sphere = sshape_element_range(&buf);
            const sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&buf);
            const sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&buf);
            bindings = (sg_bindings){
                .vertex_buffers[0] = sg_make_buffer(&vbuf_desc),
                .index_buffer = sg_make_buffer(&ibuf_desc),
            };
        }

        void Render(const vs_params_t &vs_params, const fs_params_t &fs_params)
        {
            sg_apply_pipeline(pipeline);
            sg_apply_bindings(&bindings);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE_REF(vs_params));
            sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE_REF(fs_params));
            sg_draw(sphere.base_element, sphere.num_elements, 1);
        }

        sshape_element_range_t sphere;
    };
}