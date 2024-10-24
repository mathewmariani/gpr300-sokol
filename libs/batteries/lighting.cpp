#include "lighting.h"
#include "shaders/lighting_pass.h"

namespace batteries
{
    void create_lighting_pass(lighting_t *pass, geometry_t *geometry)
    {
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

        pass->action = (sg_pass_action){
            .colors[0].load_action = SG_LOADACTION_CLEAR,
        };

        auto lighting_shader_desc = (sg_shader_desc){
            .vs = {
                .source = lighting_pass_vs,
            },
            .fs = {
                .source = lighting_pass_fs,
                .uniform_blocks[0] = {
                    .layout = SG_UNIFORMLAYOUT_NATIVE,
                    .size = sizeof(fs_lighting_params_t),
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
        };

        pass->pip = sg_make_pipeline({
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT2,
                    [1].format = SG_VERTEXFORMAT_FLOAT2,
                },
            },
            .shader = sg_make_shader(lighting_shader_desc),
            .depth = {
                .pixel_format = SG_PIXELFORMAT_DEPTH,
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = false,
            },
            .label = "lighting-pipeline",
        });

        // create an image sampler
        auto color_smplr = sg_make_sampler({
            .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
            .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
        });

        // apply bindings
        pass->bind = (sg_bindings){
            .vertex_buffers[0] = sg_make_buffer({
                .data = SG_RANGE(quad_vertices),
                .label = "quad-vertices",
            }),
            .fs.images = {
                [0] = geometry->position_img,
                [1] = geometry->normal_img,
                [2] = geometry->color_img,
            },
            .fs.samplers[0] = color_smplr,
        };
    }
}