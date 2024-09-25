#include "pbr.h"
#include "batteries/shaders/pbr_pass.h"

PhysicallyBasedRendering::PhysicallyBasedRendering()
{
    pip = sg_make_pipeline({
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
                .source = pbr_pass_vs,
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
                .source = pbr_pass_fs,
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
        .label = "pbr-pipeline",
    });
}

void PhysicallyBasedRendering::Render(const vs_params_t vs_params, const fs_params_t fs_params, batteries::model_t model)
{
    // apply bindings
    bind = model.mesh.bindings;

    sg_apply_pipeline(pip);
    sg_apply_bindings(&bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
    sg_draw(0, model.mesh.num_faces * 3, 1);
}