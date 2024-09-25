#include "water.h"
#include "batteries/shaders/windwaker_water.h"

Water::Water()
{
    pip = sg_make_pipeline({
        .layout = {
            .buffers[0] = sshape_vertex_buffer_layout_state(),
            .attrs = {
                [0] = sshape_position_vertex_attr_state(),
                [1] = sshape_normal_vertex_attr_state(),
                [2] = sshape_texcoord_vertex_attr_state(),
                [3] = sshape_color_vertex_attr_state(),
            },
        },
        .shader = sg_make_shader({
            .vs = {
                .source = windwaker_water_vs,
                .uniform_blocks[0] = {
                    .layout = SG_UNIFORMLAYOUT_NATIVE,
                    .size = sizeof(vs_params_t),
                    .uniforms = {
                        [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                        [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                        [2] = {.name = "cameraPos", .type = SG_UNIFORMTYPE_FLOAT3},
                        [3] = {.name = "wave.scale", .type = SG_UNIFORMTYPE_FLOAT},
                        [4] = {.name = "wave.strength", .type = SG_UNIFORMTYPE_FLOAT},
                        [5] = {.name = "wave.tiling", .type = SG_UNIFORMTYPE_FLOAT},
                        [6] = {.name = "wave.time", .type = SG_UNIFORMTYPE_FLOAT},
                        [7] = {.name = "wave.color", .type = SG_UNIFORMTYPE_FLOAT3},
                        [8] = {.name = "wave.direction", .type = SG_UNIFORMTYPE_FLOAT2},
                        [9] = {.name = "Ts", .type = SG_UNIFORMTYPE_FLOAT},
                        [10] = {.name = "Bs", .type = SG_UNIFORMTYPE_FLOAT},
                    },
                },
            },
            .fs = {
                .source = windwaker_water_fs,
                .images = {
                    [0] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                },
                .samplers = {
                    [0] = {.used = true, .sampler_type = SG_SAMPLERTYPE_FILTERING},
                },
                .image_sampler_pairs = {
                    [0] = {
                        .used = true,
                        .glsl_name = "water_texture",
                        .image_slot = 0,
                        .sampler_slot = 0,
                    },
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
        .label = "water-pipeline",
    });
}

void Water::Render(const vs_params_t vs_params, const fs_params_t fs_params, Water::water_t model)
{
    sg_apply_pipeline(pip);
    sg_apply_bindings(&model.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
    // sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
    sg_draw(model.plane.draw.base_element, model.plane.draw.num_elements, 1);
}