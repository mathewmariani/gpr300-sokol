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
        float time;
    };

    struct fs_params_t
    {
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
        pipeline = sg_make_pipeline((sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT3,
                    [1].format = SG_VERTEXFORMAT_FLOAT3,
                    [2].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader((sg_shader_desc){
                .vertex_func.source = water_vs,
                .fragment_func.source = water_fs,
                .uniform_blocks = {
                    [0] = {
                        .stage = SG_SHADERSTAGE_VERTEX,
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
                        .glsl_uniforms = {
                            [0] = {.glsl_name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            [1] = {.glsl_name = "model", .type = SG_UNIFORMTYPE_MAT4},
                            [2] = {.glsl_name = "cameraPos", .type = SG_UNIFORMTYPE_FLOAT3},
                            [3] = {.glsl_name = "scale", .type = SG_UNIFORMTYPE_FLOAT},
                            [4] = {.glsl_name = "strength", .type = SG_UNIFORMTYPE_FLOAT},
                            [5] = {.glsl_name = "time", .type = SG_UNIFORMTYPE_FLOAT},
                        },
                    },
                    [1] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(fs_params_t),
                        .glsl_uniforms = {
                            [0] = {.glsl_name = "tiling", .type = SG_UNIFORMTYPE_FLOAT},
                            [1] = {.glsl_name = "time", .type = SG_UNIFORMTYPE_FLOAT},
                            [2] = {.glsl_name = "color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [3] = {.glsl_name = "direction", .type = SG_UNIFORMTYPE_FLOAT2},
                            [4] = {.glsl_name = "Ts", .type = SG_UNIFORMTYPE_FLOAT},
                            [5] = {.glsl_name = "Bs", .type = SG_UNIFORMTYPE_FLOAT},
                            [6] = {.glsl_name = "lod_bias", .type = SG_UNIFORMTYPE_FLOAT},
                        },
                    },
                },
                .images = {
                    [0] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                },
                .samplers = {
                    [0] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sampler_type = SG_SAMPLERTYPE_FILTERING},
                },
                .image_sampler_pairs = {
                    [0] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .glsl_name = "water_texture",
                        .image_slot = 0,
                        .sampler_slot = 0,
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