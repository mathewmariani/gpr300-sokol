#pragma once

// batteries
#include "batteries/pass.h"

// glm
#include "glm/glm.hpp"

#include "water.glsl.h"

struct Water final : public batteries::Pass
{
    struct vs_params_t
    {
        glm::mat4 view_proj;
        glm::mat4 model;
        glm::vec3 camera_pos;
    };

    struct fs_params_t
    {
        float lod_bias;
        float tiling;
        float time;
        float top_scale;
        float bottom_scale;
        float brightness_lower_cutoff;
        float brightness_upper_cutoff;
    };

    Water()
    {
        pipeline = sg_make_pipeline((sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT3,
                    [1].format = SG_VERTEXFORMAT_FLOAT3,
                    [2].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader({
                .vs = {
                    .source = water_vs,
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
                    .source = water_fs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(fs_params_t),
                        .uniforms = {
                            [0] = {.name = "lod_bias", .type = SG_UNIFORMTYPE_FLOAT},
                            [1] = {.name = "tiling", .type = SG_UNIFORMTYPE_FLOAT},
                            [2] = {.name = "time", .type = SG_UNIFORMTYPE_FLOAT},
                            [3] = {.name = "top_scale", .type = SG_UNIFORMTYPE_FLOAT},
                            [4] = {.name = "bottom_scale", .type = SG_UNIFORMTYPE_FLOAT},
                            [5] = {.name = "brightness_lower_cutoff", .type = SG_UNIFORMTYPE_FLOAT},
                            [6] = {.name = "brightness_upper_cutoff", .type = SG_UNIFORMTYPE_FLOAT},
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
                            .glsl_name = "texture",
                            .image_slot = 0,
                            .sampler_slot = 0,
                        },
                    },
                },
            }),
            .index_type = SG_INDEXTYPE_UINT16,
            .cull_mode = SG_CULLMODE_NONE,
            .colors[0] = {
                .blend = {
                    .enabled = true,
                    .src_factor_rgb = SG_BLENDFACTOR_ONE,
                    .dst_factor_rgb = SG_BLENDFACTOR_ZERO,
                    .op_rgb = SG_BLENDOP_ADD,
                    .src_factor_alpha = SG_BLENDFACTOR_ONE,
                    .dst_factor_alpha = SG_BLENDFACTOR_ZERO,
                    .op_alpha = SG_BLENDOP_ADD,
                },
            },
            .depth = {
                .pixel_format = SG_PIXELFORMAT_DEPTH,
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = true,
            },
            .label = "sunshine-water-pipeline",
        });
    }
};