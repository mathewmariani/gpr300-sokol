#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/pass.h"

#include "blinnphong.glsl.h"

#define MAX_LIGHTS (64)

struct BlinnPhong final : public batteries::Pass
{
    struct my_light_t
    {
        glm::vec4 color[MAX_LIGHTS];
        glm::vec4 position[MAX_LIGHTS];
    };

    struct fs_params_t
    {
        glm::vec3 camera_position;
        my_light_t lights;
        batteries::ambient_t ambient;
        int num_instances;
    };

    BlinnPhong()
    {
        pipeline = sg_make_pipeline((sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader((sg_shader_desc){
                .vertex_func.source = blinnphong_vs,
                .fragment_func.source = blinnphong_fs,
                .uniform_blocks = {
                    [0] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(fs_params_t),
                        .glsl_uniforms = {
                            [0] = {.glsl_name = "camera_position", .type = SG_UNIFORMTYPE_FLOAT3},
                            [1] = {.glsl_name = "lights.color", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = MAX_LIGHTS},
                            [2] = {.glsl_name = "lights.position", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = MAX_LIGHTS},
                            [3] = {.glsl_name = "ambient.intensity", .type = SG_UNIFORMTYPE_FLOAT},
                            [4] = {.glsl_name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [5] = {.glsl_name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
                            [6] = {.glsl_name = "num_instances", .type = SG_UNIFORMTYPE_INT},
                        },
                    },
                },
                .images = {
                    [0].stage = SG_SHADERSTAGE_FRAGMENT,
                    [1].stage = SG_SHADERSTAGE_FRAGMENT,
                    [2].stage = SG_SHADERSTAGE_FRAGMENT,
                },
                .samplers = {[0].stage = SG_SHADERSTAGE_FRAGMENT},
                .image_sampler_pairs = {
                    [0] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .glsl_name = "g_position",
                        .image_slot = 0,
                        .sampler_slot = 0,
                    },
                    [1] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .glsl_name = "g_normal",
                        .image_slot = 1,
                        .sampler_slot = 0,
                    },
                    [2] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .glsl_name = "g_albedo",
                        .image_slot = 2,
                        .sampler_slot = 0,
                    },
                },
            }),
            .depth = {
                .pixel_format = SG_PIXELFORMAT_DEPTH,
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = false,
            },
            .label = "lighting-pipeline",
        });
    }
};