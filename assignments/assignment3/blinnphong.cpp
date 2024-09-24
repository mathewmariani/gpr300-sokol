#include "blinnphong.h"
#include "batteries/shaders/lighting_pass.h"

BlinnPhong::BlinnPhong(const batteries::Gbuffer &buffer)
{
    pip = sg_make_pipeline({
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT2,
                [1].format = SG_VERTEXFORMAT_FLOAT2,
            },
        },
        .shader = sg_make_shader({
            .vs = {
                .source = lighting_pass_vs,
            },
            .fs = {
                .source = lighting_pass_fs,
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

    // clang-format off
    float quad_vertices[] = {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f, 1.0f, 1.0f, 1.0f
    };
    // clang-format on

    // create an image sampler
    auto color_smplr = sg_make_sampler({
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
    });

    // apply bindings
    bind = (sg_bindings){
        .vertex_buffers[0] = sg_make_buffer({
            .data = SG_RANGE(quad_vertices),
            .label = "quad-vertices",
        }),
        .fs.images = {
            [0] = buffer.position_img,
            [1] = buffer.normal_img,
            [2] = buffer.color_img,
        },
        .fs.samplers[0] = color_smplr,
    };
}

void BlinnPhong::Render(const fs_params_t fs_params)
{
    sg_apply_pipeline(pip);
    sg_apply_bindings(&bind);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
    sg_draw(0, 6, 1);
}