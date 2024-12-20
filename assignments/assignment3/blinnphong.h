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
        pipeline = sg_make_pipeline({
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader({
                .vs = {
                    .source = blinnphong_vs,
                },
                .fs = {
                    .source = blinnphong_fs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(fs_params_t),
                        .uniforms = {
                            [0] = {.name = "camera_position", .type = SG_UNIFORMTYPE_FLOAT3},
                            [1] = {.name = "lights.color", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = MAX_LIGHTS},
                            [2] = {.name = "lights.position", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = MAX_LIGHTS},
                            [3] = {.name = "ambient.intensity", .type = SG_UNIFORMTYPE_FLOAT},
                            [4] = {.name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [5] = {.name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
                            [6] = {.name = "num_instances", .type = SG_UNIFORMTYPE_INT},
                        },
                    },
                    .images = {[0].used = true, [1].used = true, [2].used = true},
                    .samplers = {[0].used = true},
                    .image_sampler_pairs = {
                        [0] = {
                            .glsl_name = "g_position",
                            .image_slot = 0,
                            .sampler_slot = 0,
                            .used = true,
                        },
                        [1] = {
                            .glsl_name = "g_normal",
                            .image_slot = 1,
                            .sampler_slot = 0,
                            .used = true,
                        },
                        [2] = {
                            .glsl_name = "g_albedo",
                            .image_slot = 2,
                            .sampler_slot = 0,
                            .used = true,
                        },
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