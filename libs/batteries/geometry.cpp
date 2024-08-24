#include "geometry.h"

namespace
{
  // clang-format off
  const char geometry_pass_vs[] = R"(#version 300 es

  // attributes
  layout(location = 0) in vec3 in_position;
  layout(location = 1) in vec3 in_normal;
  layout(location = 2) in vec2 in_texcoord;
  layout(location = 3) in vec4 inst_xxxx;
  layout(location = 4) in vec4 inst_yyyy;
  layout(location = 5) in vec4 inst_zzzz;
  layout(location = 6) in vec4 inst_wwww;

  // uniforms
  uniform mat4 view_proj;

  out vec3 vs_position;
  out vec3 vs_normal;

  void main()
  {
    mat4 model = mat4(inst_xxxx, inst_yyyy, inst_zzzz, inst_wwww);
    vec4 position = model * vec4(in_position, 1.0);

    vs_position = position.xyz;
    vs_normal = transpose(inverse(mat3(model))) * in_normal;
    gl_Position = view_proj * position;
  })";
  const char geometry_pass_fs[] = R"(#version 300 es

  precision mediump float;

  // attachments
  layout(location = 0) out vec4 frag_position;
  layout(location = 1) out vec4 frag_normal;
  layout(location = 2) out vec4 frag_albedo;

  in vec3 vs_position;
  in vec3 vs_normal;

  void main()
  {
    frag_position = vec4(vs_position, 1.0);
    frag_normal = vec4(normalize(vs_normal), 0.0);
    frag_albedo = vec4(vs_normal.rgb * 0.5 + 0.5, 1.0);
  })";
  // clang-format on
}

namespace batteries
{
  void create_geometry_pass(geometry_t *pass, int width, int height)
  {
    // create 3 render target textures with different formats
    sg_image_desc img_desc = {
        .render_target = true,
        .width = width,
        .height = height,
        .sample_count = 1,
    };

    img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    pass->position_img = sg_make_image(&img_desc);

    img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    pass->normal_img = sg_make_image(&img_desc);

    img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    pass->color_img = sg_make_image(&img_desc);

    img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    pass->depth_img = sg_make_image(&img_desc);

    // create an image sampler
    auto color_smplr = sg_make_sampler((sg_sampler_desc){
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
    });

    pass->action = (sg_pass_action){
        .colors = {
            [0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 0.0f}},
            [1] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 0.0f}},
            [2] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 0.0f}},
        },
        .depth = {
            .load_action = SG_LOADACTION_CLEAR,
            .store_action = SG_STOREACTION_STORE,
            .clear_value = 1.0f,
        },
    };

    pass->attachments = sg_make_attachments({
        .colors = {
            [0].image = pass->position_img,
            [1].image = pass->normal_img,
            [2].image = pass->color_img,
        },
        .depth_stencil.image = pass->depth_img,
        .label = "geometry-pass",
    });

    auto shader_desc = (sg_shader_desc){
        .vs = {
            .source = geometry_pass_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(vs_geometry_params_t),
                .uniforms = {
                    [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                },
            },
        },
        .fs = {
            .source = geometry_pass_fs,
        },
    };

    pass->pip = sg_make_pipeline({
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT3,
                [1].format = SG_VERTEXFORMAT_FLOAT3,
                [2].format = SG_VERTEXFORMAT_FLOAT2,
                [3] = {.format = SG_VERTEXFORMAT_FLOAT4, .buffer_index = 1},
                [4] = {.format = SG_VERTEXFORMAT_FLOAT4, .buffer_index = 1},
                [5] = {.format = SG_VERTEXFORMAT_FLOAT4, .buffer_index = 1},
                [6] = {.format = SG_VERTEXFORMAT_FLOAT4, .buffer_index = 1},
            },
            .buffers = {
                [0].stride = sizeof(batteries::vertex_t),
                [1] = {
                    .stride = sizeof(glm::mat4),
                    .step_func = SG_VERTEXSTEP_PER_INSTANCE,
                },
            },
        },
        .shader = sg_make_shader(shader_desc),
        .index_type = SG_INDEXTYPE_NONE,
        .face_winding = SG_FACEWINDING_CCW,
        .cull_mode = SG_CULLMODE_BACK,
        .sample_count = 1,
        .color_count = 3,
        .colors = {
            [0].pixel_format = SG_PIXELFORMAT_RGBA8,
            [1].pixel_format = SG_PIXELFORMAT_RGBA8,
            [2].pixel_format = SG_PIXELFORMAT_RGBA8,
        },
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        .label = "display-pipeline",
    });
  }
}