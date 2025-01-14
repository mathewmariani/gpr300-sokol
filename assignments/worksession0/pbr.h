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
        pipeline = sg_make_pipeline((sg_pipeline_desc){
            .layout = {
                .attrs = {
                    // position, normal, texcoords
                    [0].format = SG_VERTEXFORMAT_FLOAT3,
                    [1].format = SG_VERTEXFORMAT_FLOAT3,
                    [2].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader((sg_shader_desc){
                .vertex_func.source = pbr_vs,
                .fragment_func.source = pbr_fs,
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
                            [3] = {.glsl_name = "camera_position", .type = SG_UNIFORMTYPE_FLOAT3},
                        },
                    },
                },
                .images = {
                    [0] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                    [1] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                    [2] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                    [3] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                    [4] = {.stage = SG_SHADERSTAGE_FRAGMENT, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
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
                        .glsl_name = "material.metallic",
                        .image_slot = 1,
                        .sampler_slot = 0,
                    },
                    [2] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .glsl_name = "material.roughness",
                        .image_slot = 2,
                        .sampler_slot = 0,
                    },
                    [3] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .glsl_name = "material.occlusion",
                        .image_slot = 3,
                        .sampler_slot = 0,
                    },
                    [4] = {
                        .stage = SG_SHADERSTAGE_FRAGMENT,
                        .glsl_name = "material.specular",
                        .image_slot = 4,
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
            .label = "pbr-pipeline",
        });
    }
};