#pragma once

// batteries
#include "batteries/pass.h"

// glm
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "water.glsl.h"

struct Water final : public batteries::Pass
{
    struct vs_params_t
    {
        glm::mat4 view_proj;
        glm::mat4 model;
        glm::vec3 camera_pos;
        float scale;
        float strength;
        float tiling;
        float time;
        glm::vec3 color;
        glm::vec2 direction;
        float top_scale;
        float bottom_scale;
        float lod_bias;
    };

    struct fs_params_t
    {
        float scale;
        float strength;
        float tiling;
        float time;
        glm::vec3 color;
        glm::vec2 direction;
        float top_scale;
        float bottom_scale;
        float lod_bias;
    };

    Water()
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
                    .source = water_vs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
                        .uniforms = {
                            [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                            [2] = {.name = "cameraPos", .type = SG_UNIFORMTYPE_FLOAT3},
                            [3] = {.name = "wave.scale", .type = SG_UNIFORMTYPE_FLOAT},
                            [4] = {.name = "wave.strength", .type = SG_UNIFORMTYPE_FLOAT},
                            [5] = {.name = "wave.tiling", .type = SG_UNIFORMTYPE_FLOAT},
                            [6] = {.name = "wave.time", .type = SG_UNIFORMTYPE_FLOAT},
                            [7] = {.name = "wave.color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [8] = {.name = "wave.direction", .type = SG_UNIFORMTYPE_FLOAT2},
                            [9] = {.name = "Ts", .type = SG_UNIFORMTYPE_FLOAT},
                            [10] = {.name = "Bs", .type = SG_UNIFORMTYPE_FLOAT},
                            [11] = {.name = "lod_bias", .type = SG_UNIFORMTYPE_FLOAT},
                        },
                    },
                },
                .fs = {
                    .source = water_fs,
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
            .label = "windwaker-water-pipeline",
        });
    }
};