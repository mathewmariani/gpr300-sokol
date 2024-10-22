#pragma once

// batteries
#include "batteries/pass.h"
#include "batteries/shape.h"
#include "batteries/texture.h"

#include "batteries/shaders/sunshine_water.h"

struct Water final : public batteries::Pass
{
    struct water_t
    {
        batteries::Texture texture;
        batteries::shape_t plane;
        sg_sampler sampler;
    };

    struct vs_params_t
    {
        glm::mat4 view_proj;
        glm::mat4 model;
        glm::vec3 camera_pos;
    };

    struct fs_params_t
    {
        float lod_bias;
    };

    Water()
    {
        pipeline = sg_make_pipeline({
            .layout = {
                .buffers[0] = sshape_vertex_buffer_layout_state(),
                .attrs = {
                    [0] = sshape_position_vertex_attr_state(),
                    [1] = sshape_normal_vertex_attr_state(),
                    [2] = sshape_texcoord_vertex_attr_state(),
                    [3] = sshape_color_vertex_attr_state(),
                },
            },
            .shader = sg_make_shader({
                .vs = {
                    .source = sunshine_water_vs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
                        .uniforms = {
                            [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                            [2] = {.name = "cameraPos", .type = SG_UNIFORMTYPE_FLOAT3},
                        },
                    },
                },
                .fs = {
                    .source = sunshine_water_fs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(fs_params_t),
                        .uniforms = {
                            [0] = {.name = "lod_bias", .type = SG_UNIFORMTYPE_FLOAT},
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
                            .glsl_name = "water_texture",
                            .image_slot = 0,
                            .sampler_slot = 0,
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
            .label = "sunshine-water-pipeline",
        });
    }
};