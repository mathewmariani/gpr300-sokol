#pragma once

// batteries
#include "batteries/pass.h"
#include "batteries/shape.h"

// glm
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include "island_generator.glsl.h"

struct Terrain final : public batteries::Pass
{
    struct vs_params_t
    {
        glm::mat4 view_proj;
        glm::mat4 model;
        glm::vec3 camera_pos;
        float scale;
    };

    Terrain()
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
                .vertex_func.source = island_generator_vs,
                .fragment_func.source = island_generator_fs,
                .uniform_blocks = {
                    [0] = {
                        .stage = SG_SHADERSTAGE_VERTEX,
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
                        .glsl_uniforms = {
                            [0] = {.glsl_name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            [1] = {.glsl_name = "model", .type = SG_UNIFORMTYPE_MAT4},
                            [2] = {.glsl_name = "cameraPos", .type = SG_UNIFORMTYPE_FLOAT3},
                            [3] = {.glsl_name = "landmass.scale", .type = SG_UNIFORMTYPE_FLOAT},
                        },
                    },
                },
                .images = {
                    [0] = {.stage = SG_SHADERSTAGE_VERTEX, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                    [1] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                },
                .samplers = {
                    [0] = {.stage = SG_SHADERSTAGE_VERTEX, .sampler_type = SG_SAMPLERTYPE_FILTERING},
                    [1] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sampler_type = SG_SAMPLERTYPE_FILTERING},
                },
                .image_sampler_pairs = {
                    [0] = {
                        .stage = SG_SHADERSTAGE_VERTEX,
                        .glsl_name = "heightmap",
                        .image_slot = 0,
                        .sampler_slot = 0,
                    },
                    [1] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .glsl_name = "heightmap",
                        .image_slot = 1,
                        .sampler_slot = 1,
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
            .label = "terrain-pipeline",
        });
    }
};