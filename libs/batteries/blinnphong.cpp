#include "blinnphong.h"

namespace
{
  // clang-format off
const char blinn_phong_vs[] = R"(#version 300 es

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_texcoord;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

// send to fragment
out vec3 world_position;
out vec3 world_normal;

void main()
{
  world_position = vec3(model * vec4(in_position, 1.0));
  world_normal = mat3(transpose(inverse(model))) * in_normal;
  gl_Position = view_proj * model * vec4(in_position, 1.0);
})";
const char blinn_phong_fs[] = R"(#version 300 es

precision mediump float;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};
struct Light {
  float brightness;
  vec3 color;
  vec3 position;
};

out vec4 FragColor;

in vec3 world_position;
in vec3 world_normal;

uniform Material material;
uniform Light light;
uniform vec3 camera_position;

void main()
{
  // Make sure fragment normal is still length 1 after interpolation.
  vec3 normal = normalize(world_normal);

  // The angle the light ray touches the fragment.
  vec3 light_direction = normalize(light.position - world_position);

  // The angle the looks at the fragment.
  vec3 camera_direction = normalize(camera_position - world_position);

  // Exactly halfway between the camera direction and the light direction.
  vec3 halfway_direction = normalize(light_direction + camera_direction);

  // Calculate diffuse lighting
  float diffuse = max(dot(normal, light_direction), 0.0);;

  // Calculate specular lighting
  float specular = 0.0;
  if (diffuse != 0.0)
  {
    specular = pow(max(dot(normal, halfway_direction), 0.0), material.shininess);
  }

  // Calculate final lighting color
  vec3 light_color = (material.ambient + (material.diffuse * diffuse) + (material.specular * specular)) * light.color;
  vec3 object_color = vec3(normal * 0.5 + 0.5);
  vec3 result = object_color * light_color;

  FragColor = vec4(result, 1.0);
})";
  // clang-format on
}

namespace batteries
{
  void create_blinnphong_pass(blinnphong_t *pass)
  {
    auto shader_desc = (sg_shader_desc){
        .vs = {
            .source = blinn_phong_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(batteries::vs_blinnphong_params_t),
                .uniforms = {
                    [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                    [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                },
            },
        },
        .fs = {
            .source = blinn_phong_fs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(batteries::fs_blinnphong_params_t),
                .uniforms = {
                    [0] = {.name = "material.ambient", .type = SG_UNIFORMTYPE_FLOAT3},
                    [1] = {.name = "material.diffuse", .type = SG_UNIFORMTYPE_FLOAT3},
                    [2] = {.name = "material.specular", .type = SG_UNIFORMTYPE_FLOAT3},
                    [3] = {.name = "material.shininess", .type = SG_UNIFORMTYPE_FLOAT},
                    [4] = {.name = "light.brightness", .type = SG_UNIFORMTYPE_FLOAT},
                    [5] = {.name = "light.color", .type = SG_UNIFORMTYPE_FLOAT3},
                    [6] = {.name = "light.position", .type = SG_UNIFORMTYPE_FLOAT3},
                    [7] = {.name = "camera_position", .type = SG_UNIFORMTYPE_FLOAT3},
                },
            },
        },
    };

    pass->action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        },
    };

    pass->pip = sg_make_pipeline({
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT3,
                [1].format = SG_VERTEXFORMAT_FLOAT3,
                [2].format = SG_VERTEXFORMAT_FLOAT2,
            },
        },
        .shader = sg_make_shader(shader_desc),
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
        .label = "blinnphong-pipeline",
    });
  }
}