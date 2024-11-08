#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/pass.h"

// glm
#include "glm/mat4x4.hpp"

#include "toonshading.glsl.h"

struct ToonShading final : public batteries::Pass
{
    struct Palette
    {
        glm::vec3 highlight;
        glm::vec3 shadow;
    };

    struct vs_params_t
    {
        glm::mat4 view_proj;
        glm::mat4 model;
    };

    struct fs_params_t
    {
        batteries::light_t light;
        Palette palette;
    };

    ToonShading()
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
                .vertex_func.source = toonshading_vs,
                .fragment_func.source = toonshading_fs,
                .uniform_blocks = {
                    [0] = {
                        .stage = SG_SHADERSTAGE_VERTEX,
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
                        .glsl_uniforms = {
                            [0] = {.glsl_name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            [1] = {.glsl_name = "model", .type = SG_UNIFORMTYPE_MAT4},
                        },
                    },
                    [1] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(fs_params_t),
                        .glsl_uniforms = {
                            [0] = {.glsl_name = "light.brightness", .type = SG_UNIFORMTYPE_FLOAT},
                            [1] = {.glsl_name = "light.color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [2] = {.glsl_name = "light.position", .type = SG_UNIFORMTYPE_FLOAT3},
                            [3] = {.glsl_name = "palette.highlight", .type = SG_UNIFORMTYPE_FLOAT3},
                            [4] = {.glsl_name = "palette.shadow", .type = SG_UNIFORMTYPE_FLOAT3},
                        },
                    },
                },
                .images = {
                    [0] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                    [1] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                },
                .samplers = {
                    [0] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sampler_type = SG_SAMPLERTYPE_FILTERING},
                },
                .image_sampler_pairs = {
                    [0] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .glsl_name = "material.albedo",
                        .image_slot = 0,
                        .sampler_slot = 0,
                    },
                    [1] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .glsl_name = "material.zatoon",
                        .image_slot = 1,
                        .sampler_slot = 0,
                    },
                },
            }),
            .index_type = SG_INDEXTYPE_UINT16,
            .face_winding = SG_FACEWINDING_CCW,
            .cull_mode = SG_CULLMODE_BACK,
            .colors = {
                [0].pixel_format = SG_PIXELFORMAT_RGBA8,
            },
            .depth = {
                .pixel_format = SG_PIXELFORMAT_DEPTH,
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = true,
            },
            .label = "toonshadowing-pipeline",
        });
    }
};