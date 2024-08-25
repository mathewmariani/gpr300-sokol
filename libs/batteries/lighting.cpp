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

    auto quad_buffer = sg_make_buffer({
        .data = SG_RANGE(quad_vertices),
        .label = "quad-vertices",
    });

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
                    [0] = {.name = "eye", .type = SG_UNIFORMTYPE_FLOAT3},
                    [1] = {.name = "ambient.intensity", .type = SG_UNIFORMTYPE_FLOAT},
                    [2] = {.name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                    [3] = {.name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
                    [4] = {.name = "material.ambient", .type = SG_UNIFORMTYPE_FLOAT3},
                    [5] = {.name = "material.diffuse", .type = SG_UNIFORMTYPE_FLOAT3},
                    [6] = {.name = "material.specular", .type = SG_UNIFORMTYPE_FLOAT3},
                    [7] = {.name = "material.shininess", .type = SG_UNIFORMTYPE_FLOAT},
                    [8] = {.name = "light.radius", .type = SG_UNIFORMTYPE_FLOAT, .array_count = 64},
                    [9] = {.name = "light.color", .type = SG_UNIFORMTYPE_FLOAT3, .array_count = 64},
                    [10] = {.name = "light.position", .type = SG_UNIFORMTYPE_FLOAT3, .array_count = 64},
                    [11] = {.name = "imgui.radius", .type = SG_UNIFORMTYPE_FLOAT},
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
        .vertex_buffers[0] = quad_buffer,
        .fs.images = {
            [0] = geometry->position_img,
            [1] = geometry->normal_img,
            [2] = geometry->color_img,
        },
        .fs.samplers[0] = color_smplr,
    };
  }
}