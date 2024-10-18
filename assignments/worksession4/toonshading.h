#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/pass.h"

#include "batteries/shaders/toon_shadowing.h"

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
                    .source = toon_shadowing_vs,
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
                    .source = toon_shadowing_fs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(fs_params_t),
                        .uniforms = {
                            [0] = {.name = "light.brightness", .type = SG_UNIFORMTYPE_FLOAT},
                            [1] = {.name = "light.color", .type = SG_UNIFORMTYPE_FLOAT3},
                            [2] = {.name = "light.position", .type = SG_UNIFORMTYPE_FLOAT3},
                            [3] = {.name = "palette.highlight", .type = SG_UNIFORMTYPE_FLOAT3},
                            [4] = {.name = "palette.shadow", .type = SG_UNIFORMTYPE_FLOAT3},
                        },
                    },
                    .images = {
                        [0] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                        [1] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
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
                            .glsl_name = "material.zatoon",
                            .image_slot = 1,
                            .sampler_slot = 0,
                        },
                    },
                },
            }),
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
            .label = "toonshadowing-pipeline",
        });
    }
};