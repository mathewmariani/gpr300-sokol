#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/texture.h"
#include "batteries/model.h"
#include "batteries/pass.h"

#include "pbr.glsl.h"

struct PhysicallyBasedRendering final : public batteries::Pass
{
    struct Material
    {
        batteries::Texture col;
        batteries::Texture mtl;
        batteries::Texture rgh;
        batteries::Texture ao;
        batteries::Texture spc;
    };

    struct vs_params_t
    {
        glm::mat4 view_proj;
        glm::mat4 model;
    };

    struct fs_params_t
    {
        batteries::light_t light;
        glm::vec3 camera_position;
    };

    PhysicallyBasedRendering()
    {
        pipeline = sg_make_pipeline({
            .layout = {
                .attrs = {
                    // position, normal, texcoords
                    [0].format = SG_VERTEXFORMAT_FLOAT3,
                    [1].format = SG_VERTEXFORMAT_FLOAT3,
                    [2].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader({
                .vs = {
                    .source = pbr_vs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
                        .uniforms = {
                            [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                        },
                    },
                },
                .fs = {
                    .source = pbr_fs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(fs_params_t),
                        .uniforms = {
                            [0] = {.name = "light.brightness", .type = SG_UNIFORMTYPE_FLOAT},
                            [1] = {.name = "light.color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [2] = {.name = "light.position", .type = SG_UNIFORMTYPE_FLOAT3},
                            [3] = {.name = "camera_position", .type = SG_UNIFORMTYPE_FLOAT3},
                        },
                    },
                    .images = {
                        [0] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                        [1] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                        [2] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                        [3] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                        [4] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                    },
                    .samplers = {
                        [0] = {.used = true, .sampler_type = SG_SAMPLERTYPE_FILTERING},
                    },
                    .image_sampler_pairs = {
                        [0] = {
                            .used = true,
                            .glsl_name = "material.albedo",
                            .image_slot = 0,
                            .sampler_slot = 0,
                        },
                        [1] = {
                            .used = true,
                            .glsl_name = "material.metallic",
                            .image_slot = 1,
                            .sampler_slot = 0,
                        },
                        [2] = {
                            .used = true,
                            .glsl_name = "material.roughness",
                            .image_slot = 2,
                            .sampler_slot = 0,
                        },
                        [3] = {
                            .used = true,
                            .glsl_name = "material.occlusion",
                            .image_slot = 3,
                            .sampler_slot = 0,
                        },
                        [4] = {
                            .used = true,
                            .glsl_name = "material.specular",
                            .image_slot = 4,
                            .sampler_slot = 0,
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
            .label = "pbr-pipeline",
        });
    }
};