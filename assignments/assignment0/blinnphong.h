#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/pass.h"

// shader
#include "blinnphong.glsl.h"

struct BlinnPhong final : public batteries::Pass
{
    struct vs_params_t
    {
        glm::mat4 view_proj;
        glm::mat4 model;
    };

    struct fs_params_t
    {
        batteries::material_t material;
        batteries::light_t light;
        batteries::ambient_t ambient;
        glm::vec3 camera_position;
    };

    BlinnPhong()
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
                .vertex_func.source = blinnphong_vs,
                .fragment_func.source = blinnphong_fs,
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
                            [0] = {.glsl_name = "material.ambient", .type = SG_UNIFORMTYPE_FLOAT3},
                            [1] = {.glsl_name = "material.diffuse", .type = SG_UNIFORMTYPE_FLOAT3},
                            [2] = {.glsl_name = "material.specular", .type = SG_UNIFORMTYPE_FLOAT3},
                            [3] = {.glsl_name = "material.shininess", .type = SG_UNIFORMTYPE_FLOAT},
                            [4] = {.glsl_name = "light.brightness", .type = SG_UNIFORMTYPE_FLOAT},
                            [5] = {.glsl_name = "light.color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [6] = {.glsl_name = "light.position", .type = SG_UNIFORMTYPE_FLOAT3},
                            [7] = {.glsl_name = "ambient.intensity", .type = SG_UNIFORMTYPE_FLOAT},
                            [8] = {.glsl_name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [9] = {.glsl_name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
                            [10] = {.glsl_name = "camera_position", .type = SG_UNIFORMTYPE_FLOAT3},
                        },
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
            .label = "blinnphong-pipeline",
        });
    }
};