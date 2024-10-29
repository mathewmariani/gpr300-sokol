#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/pass.h"

#include "shadow_map.glsl.h"

struct Shadow final : public batteries::Pass
{
    struct vs_params_t
    {
        glm::mat4 model;
        glm::mat4 view_proj;
        glm::mat4 light_view_proj;
    };

    struct fs_params_t
    {
        batteries::material_t material;
        batteries::light_t light;
        batteries::ambient_t ambient;
        glm::vec3 camera_position;
    };

    Shadow()
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
                    .source = shadow_map_vs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
                        .uniforms = {
                            [0] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                            [1] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            [2] = {.name = "light_view_proj", .type = SG_UNIFORMTYPE_MAT4},
                        },
                    },
                },
                .fs = {
                    .source = shadow_map_fs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(fs_params_t),
                        .uniforms = {
                            [0] = {.name = "material.ambient", .type = SG_UNIFORMTYPE_FLOAT3},
                            [1] = {.name = "material.diffuse", .type = SG_UNIFORMTYPE_FLOAT3},
                            [2] = {.name = "material.specular", .type = SG_UNIFORMTYPE_FLOAT3},
                            [3] = {.name = "material.shininess", .type = SG_UNIFORMTYPE_FLOAT},
                            [4] = {.name = "light.brightness", .type = SG_UNIFORMTYPE_FLOAT},
                            [5] = {.name = "light.color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [6] = {.name = "light.position", .type = SG_UNIFORMTYPE_FLOAT3},
                            [7] = {.name = "ambient.intensity", .type = SG_UNIFORMTYPE_FLOAT},
                            [8] = {.name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [9] = {.name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
                            [10] = {.name = "camera_position", .type = SG_UNIFORMTYPE_FLOAT3},
                        },
                    },
                    .images[0] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_DEPTH},
                    .samplers[0] = {.used = true, .sampler_type = SG_SAMPLERTYPE_COMPARISON},
                    .image_sampler_pairs[0] = {
                        .glsl_name = "shadow_map",
                        .image_slot = 0,
                        .sampler_slot = 0,
                        .used = true,
                    },
                },
            }),
            // .index_type = SG_INDEXTYPE_UINT16,
            .index_type = SG_INDEXTYPE_NONE,
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
            .label = "shadowmap-pipeline",
        });
    }
};