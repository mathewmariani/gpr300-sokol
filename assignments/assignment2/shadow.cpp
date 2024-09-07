#include "shadow.h"
#include "batteries/shaders/shadow_map.h"

Shadow::Shadow()
{
    pip = sg_make_pipeline({
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
        .index_type = SG_INDEXTYPE_NONE,
        .face_winding = SG_FACEWINDING_CCW,
        .cull_mode = SG_CULLMODE_BACK,
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        .label = "shadow-pipeline",
    });

    sampler = sg_make_sampler({
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .compare = SG_COMPAREFUNC_LESS,
        .label = "shadow-sampler",
    });
}

void Shadow::Render(const vs_params_t vs_params, const fs_params_t fs_params, sg_image depth, batteries::model_t model)
{
    auto bindings = (sg_bindings){
        .vertex_buffers = model.mesh.vbuf,
        .fs = {
            .images[0] = depth,
            .samplers[0] = sampler,
        },
    };

    sg_apply_pipeline(pip);
    sg_apply_bindings(&bindings);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
    sg_draw(0, model.mesh.num_faces * 3, 1);
}