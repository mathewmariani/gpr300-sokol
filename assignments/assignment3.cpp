#define BATTERIES_IMPL
#include "batteries.h"

//
// Assignment3 -- Deferred Rendering
//

// shaders
#include "shaders/geometry_pass.h"
#include "shaders/lighting_pass.h"
#include "shaders/gizmo_light.h"
#include "shaders/quad.h"

#include <string>

enum
{
  OFFSCREEN_WIDTH = 1024,
  OFFSCREEN_HEIGHT = 1024,
  MAX_INSTANCES = 128,
  MAX_LIGHTS = 25,

  // attributes
  ATTR_vs_position = 0,
  ATTR_vs_normal = 0,
  ATTR_vs_texcoord = 0,
  SLOT_fs_point_lights = 0,
};

typedef struct
{
  glm::mat4 view_proj;
} vs_geometry_params_t;

typedef struct
{
  glm::mat4 view_proj;
  glm::mat4 model;
} vs_gizmo_params_t;

typedef struct
{
  glm::vec3 light_color;
} fs_gizmo_light_params_t;

struct pointlight_t
{
  glm::vec4 radius[MAX_LIGHTS];
  glm::vec4 color[MAX_LIGHTS];
  glm::vec4 position[MAX_LIGHTS];
};

typedef struct
{
  glm::vec3 eye;
  batteries::ambient_t ambient;
  batteries::material_t material;
  pointlight_t lights;
} fs_lighting_params_t;

// application state
static struct
{
  struct
  {
    sg_pass_action pass_action;
    sg_attachments attachments;
    sg_pipeline pip;
    sg_bindings bind;

    sg_image color_img;
    sg_image position_img;
    sg_image normal_img;
    sg_image depth_img;
  } geometry;

  struct
  {
    sg_pass_action pass_action;
    sg_attachments attachments;
    sg_pipeline pip;
    sg_bindings bind;
    sg_image img;
  } lighting;

  struct
  {
    sg_pass_action pass_action;
    sg_attachments attachments;
    sg_pipeline pip;
    sg_bindings bind;
    batteries::shape_t shape;
  } gizmo;

  struct
  {
    simgui_image_t color_img;
    simgui_image_t position_img;
    simgui_image_t normal_img;
    simgui_image_t depth_img;
  } debug;

  uint8_t file_buffer[batteries::megabytes(4)];

  int num_instances;
  batteries::camera_t camera;
  batteries::camera_controller_t camera_controller;
  batteries::model_t suzanne;
  batteries::material_t material;
  batteries::ambient_t ambient;

} state = {
    .num_instances = 1,
    .ambient = {
        .color = glm::vec3(0.25f, 0.45f, 0.65f),
    },
    .material = {
        .Ka = 1.0f,
        .Kd = 0.5f,
        .Ks = 0.5f,
        .Shininess = 128.0f,
    },
};

// instance data buffer;
static glm::mat4 instance_data[MAX_INSTANCES];

static pointlight_t instance_light_data;

void load_suzanne(void)
{
  state.suzanne.mesh.vbuf = sg_alloc_buffer();
  batteries::assets::load_obj((batteries::assets::obj_request_t){
      .buffer_id = state.suzanne.mesh.vbuf,
      .mesh = &state.suzanne.mesh,
      .path = "assets/suzanne.obj",
      .buffer = SG_RANGE(state.file_buffer),
  });
}

static void init_instance_data(void)
{
  for (int i = 0, x = 0, y = 0, dx = 0, dy = 0; i < MAX_INSTANCES; i++, x += dx, y += dy)
  {
    glm::mat4 *inst = &instance_data[i];
    *inst = glm::translate(glm::vec3(x * 3.0f, 0.0f, y * 3.0f));

    // at a corner?
    if (abs(x) == abs(y))
    {
      if (x >= 0)
      {
        // top-right corner: start a new ring
        if (y >= 0)
        {
          x += 1;
          y += 1;
          dx = 0;
          dy = -1;
        }
        // bottom-right corner
        else
        {
          dx = -1;
          dy = 0;
        }
      }
      else
      {
        // top-left corner
        if (y >= 0)
        {
          dx = +1;
          dy = 0;
        }
        // bottom-left corner
        else
        {
          dx = 0;
          dy = +1;
        }
      }
    }
  }
}

static glm::vec3 generateRandomPointOnUnitSphere()
{
  double u = static_cast<double>(rand()) / RAND_MAX;
  double v = static_cast<double>(rand()) / RAND_MAX;

  double phi = 2.0 * M_PI * u;
  double theta = acos(2.0 * v - 1.0);

  double x = sin(theta) * cos(phi);
  double y = sin(theta) * sin(phi);
  double z = cos(theta);

  return glm::vec3(x, y, z);
}
static const glm::vec3 light_colors[5] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
};

static void init_instance_lights(void)
{
  for (int i = 0, x = 0, y = 0, dx = 0, dy = 0; i < MAX_LIGHTS; i++, x += dx, y += dy)
  {
    const auto offset = generateRandomPointOnUnitSphere();
    const auto position = glm::vec3(x * 3.0f, 0.0f, y * 3.0f);

    auto r = static_cast<float>(((rand() % 100) / 200.0f) + 0.5);
    auto g = static_cast<float>(((rand() % 100) / 200.0f) + 0.5);
    auto b = static_cast<float>(((rand() % 100) / 200.0f) + 0.5);

    instance_light_data.radius[i] = glm::vec4(10.0f, 0.0f, 0.0f, 1.0f);
    // instance_light_data.color[i] = glm::vec4(light_colors[rand() % 5], 1.0f);
    instance_light_data.color[i] = glm::vec4(r, g, b, 1.0f);
    instance_light_data.position[i] = glm::vec4(position + offset, 1.0f);

    // at a corner?
    if (abs(x) == abs(y))
    {
      if (x >= 0)
      {
        // top-right corner: start a new ring
        if (y >= 0)
        {
          x += 1;
          y += 1;
          dx = 0;
          dy = -1;
        }
        // bottom-right corner
        else
        {
          dx = -1;
          dy = 0;
        }
      }
      else
      {
        // top-left corner
        if (y >= 0)
        {
          dx = +1;
          dy = 0;
        }
        // bottom-left corner
        else
        {
          dx = 0;
          dy = +1;
        }
      }
    }
  }
}

void create_geometry_pass(void)
{
  // create 3 render target textures with different formats
  sg_image_desc img_desc = {
      .render_target = true,
      .width = OFFSCREEN_WIDTH,
      .height = OFFSCREEN_HEIGHT,
      .sample_count = 1,
  };

  img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
  state.geometry.position_img = sg_make_image(&img_desc);

  img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
  state.geometry.normal_img = sg_make_image(&img_desc);

  img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
  state.geometry.color_img = sg_make_image(&img_desc);

  img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
  state.geometry.depth_img = sg_make_image(&img_desc);

  // create an image sampler
  auto color_smplr = sg_make_sampler({
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
  });

  state.geometry.pass_action = (sg_pass_action){
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

  auto attachment_desc = (sg_attachments_desc){
      .colors = {
          [0].image = state.geometry.position_img,
          [1].image = state.geometry.normal_img,
          [2].image = state.geometry.color_img,
      },
      .depth_stencil.image = state.geometry.depth_img,
      .label = "geometry-attachment",
  };
  state.geometry.attachments = sg_make_attachments(&attachment_desc);

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

  state.geometry.pip = sg_make_pipeline({
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

  init_instance_data();
  sg_buffer_desc buf_desc = {
      .type = SG_BUFFERTYPE_VERTEXBUFFER,
      .data = SG_RANGE(instance_data),
  };

  state.geometry.bind = (sg_bindings){
      .vertex_buffers = {
          [0] = state.suzanne.mesh.vbuf,
          [1] = sg_make_buffer(&buf_desc),
      },
  };
}

void create_lighting_pass(void)
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

  // create 3 render target textures with different formats
  sg_image_desc img_desc = {
      .render_target = true,
      .width = OFFSCREEN_WIDTH,
      .height = OFFSCREEN_HEIGHT,
      .sample_count = 1,
  };

  img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
  state.geometry.position_img = sg_make_image(&img_desc);

  img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
  state.geometry.normal_img = sg_make_image(&img_desc);

  img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
  state.geometry.color_img = sg_make_image(&img_desc);

  img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
  state.geometry.depth_img = sg_make_image(&img_desc);

  // create an image sampler
  auto color_smplr = sg_make_sampler({
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
  });

  state.geometry.pass_action = (sg_pass_action){
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

  init_instance_lights();

  state.lighting.pass_action = (sg_pass_action){
      .colors[0].load_action = SG_LOADACTION_CLEAR,
      .depth.load_action = SG_LOADACTION_DONTCARE,
      .stencil.load_action = SG_LOADACTION_DONTCARE,
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
                  [1] = {.name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
                  [2] = {.name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                  [3] = {.name = "material.Ka", .type = SG_UNIFORMTYPE_FLOAT},
                  [4] = {.name = "material.Kd", .type = SG_UNIFORMTYPE_FLOAT},
                  [5] = {.name = "material.Ks", .type = SG_UNIFORMTYPE_FLOAT},
                  [6] = {.name = "material.Shininess", .type = SG_UNIFORMTYPE_FLOAT},
                  [7] = {.name = "lights.radius", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = MAX_LIGHTS},
                  [8] = {.name = "lights.color", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = MAX_LIGHTS},
                  [9] = {.name = "lights.position", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = MAX_LIGHTS},
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

  state.lighting.pip = sg_make_pipeline({
      .layout = {
          .attrs = {
              [0].format = SG_VERTEXFORMAT_FLOAT2,
              [1].format = SG_VERTEXFORMAT_FLOAT2,
          },
      },
      .shader = sg_make_shader(lighting_shader_desc),
      .label = "display-pipeline",
  });

  // create an image sampler
  auto color_smplr = sg_make_sampler({
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
  });

  // apply bindings
  state.lighting.bind = (sg_bindings){
      .vertex_buffers[0] = quad_buffer,
      .fs.images = {
          [0] = state.geometry.position_img,
          [1] = state.geometry.normal_img,
          [2] = state.geometry.color_img,
      },
      .fs.samplers[0] = color_smplr,
  };
}

void create_gizmo_pass(void)
{
  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = gizmo_light_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_gizmo_params_t),
              .uniforms = {
                  [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                  [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
              },
          },
      },
      .fs = {
          .source = gizmo_light_fs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(fs_gizmo_light_params_t),
              .uniforms = {
                  [0] = {.name = "light_color", .type = SG_UNIFORMTYPE_FLOAT3},
              },
          },
      },
  };

  state.gizmo.pass_action = (sg_pass_action){
      .colors[0].load_action = SG_LOADACTION_DONTCARE,
      .depth.load_action = SG_LOADACTION_LOAD,
      .stencil.load_action = SG_LOADACTION_DONTCARE,
  };

  auto attachment_desc = (sg_attachments_desc){
      // .colors[0].image
      .depth_stencil.image = state.geometry.depth_img,
      .label = "gizmo-attachment",
  };
  state.gizmo.attachments = sg_make_attachments(&attachment_desc);

  state.gizmo.pip = sg_make_pipeline({
      .layout = {
          .buffers[0] = sshape_vertex_buffer_layout_state(),
          .attrs = {
              [0] = sshape_position_vertex_attr_state(),
              [1] = sshape_normal_vertex_attr_state(),
              [2] = sshape_texcoord_vertex_attr_state(),
              [3] = sshape_color_vertex_attr_state(),
          },
      },
      .shader = sg_make_shader(shader_desc),
      .index_type = SG_INDEXTYPE_UINT16,
      .cull_mode = SG_CULLMODE_NONE,
      .color_count = 0,
      .sample_count = 0,
      .depth = {
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .label = "gizmo-pipeline",
  });

  // generate shape geometries
  sshape_vertex_t vertices[2 * 1024];
  uint16_t indices[4 * 1024];
  sshape_buffer_t buf = {
      .vertices.buffer = SSHAPE_RANGE(vertices),
      .indices.buffer = SSHAPE_RANGE(indices),
  };

  const sshape_sphere_t sphere = {
      .radius = 0.25f,
  };
  buf = sshape_build_sphere(&buf, &sphere);

  state.gizmo.shape.draw = sshape_element_range(&buf);
  state.gizmo.shape.transform.position = glm::vec3(0.0f, -1.0f, 0.0f);

  // one vertex/index-buffer-pair for all shapes
  const auto vbuf_desc = sshape_vertex_buffer_desc(&buf);
  const auto ibuf_desc = sshape_index_buffer_desc(&buf);

  state.gizmo.bind = (sg_bindings){
      .vertex_buffers[0] = sg_make_buffer(&vbuf_desc),
      .index_buffer = sg_make_buffer(&ibuf_desc),
  };
}

void create_display_pass(void)
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

  state.lighting.pass_action = (sg_pass_action){
      .colors[0].load_action = SG_LOADACTION_CLEAR,
      .depth.load_action = SG_LOADACTION_DONTCARE,
      .stencil.load_action = SG_LOADACTION_DONTCARE,
  };

  auto quad_buffer = sg_make_buffer({
      .data = SG_RANGE(quad_vertices),
      .label = "quad-vertices",
  });

  auto display_shader_desc = (sg_shader_desc){
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
                  [1] = {.name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
                  [2] = {.name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                  [3] = {.name = "material.Ka", .type = SG_UNIFORMTYPE_FLOAT},
                  [4] = {.name = "material.Kd", .type = SG_UNIFORMTYPE_FLOAT},
                  [5] = {.name = "material.Ks", .type = SG_UNIFORMTYPE_FLOAT},
                  [6] = {.name = "material.Shininess", .type = SG_UNIFORMTYPE_FLOAT},
                  [7] = {.name = "lights.radius", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = MAX_LIGHTS},
                  [8] = {.name = "lights.color", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = MAX_LIGHTS},
                  [9] = {.name = "lights.position", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = MAX_LIGHTS},
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

  state.lighting.pip = sg_make_pipeline({
      .layout = {
          .attrs = {
              [0].format = SG_VERTEXFORMAT_FLOAT2,
              [1].format = SG_VERTEXFORMAT_FLOAT2,
          },
      },
      .shader = sg_make_shader(lighting_shader_desc),
      .label = "display-pipeline",
  });

  // create an image sampler
  auto color_smplr = sg_make_sampler({
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
  });

  // apply bindings
  state.lighting.bind = (sg_bindings){
      .vertex_buffers[0] = quad_buffer,
      .fs.images = {
          [0] = state.geometry.position_img,
          [1] = state.geometry.normal_img,
          [2] = state.geometry.color_img,
      },
      .fs.samplers[0] = color_smplr,
  };
}

void init(void)
{
  batteries::setup();

  load_suzanne();
  create_geometry_pass();
  create_lighting_pass();
  create_gizmo_pass();
  create_display_pass();

  // create an sokol-imgui wrapper for the shadow map
  auto dbg_smp = sg_make_sampler({
      .min_filter = SG_FILTER_NEAREST,
      .mag_filter = SG_FILTER_NEAREST,
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .label = "ui-sampler",
  });
  // create an sokol-imgui wrapper for geometry
  state.debug.color_img = simgui_make_image({
      .image = state.geometry.color_img,
      .sampler = dbg_smp,
  });
  state.debug.position_img = simgui_make_image({
      .image = state.geometry.position_img,
      .sampler = dbg_smp,
  });
  state.debug.normal_img = simgui_make_image({
      .image = state.geometry.normal_img,
      .sampler = dbg_smp,
  });
  state.debug.depth_img = simgui_make_image({
      .image = state.geometry.depth_img,
      .sampler = dbg_smp,
  });
}

void draw_ui(void)
{
  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  ImGui::SliderInt("Num Instances", &state.num_instances, 1, MAX_INSTANCES);

  if (ImGui::ColorEdit3("Ambient Light", &state.ambient.color[0]))
  {
    // state.display.pass_action.colors[0].clear_value = {state.scene.ambient_light.r, state.scene.ambient_light.g, state.scene.ambient_light.b, 1.0f};
  }
  if (ImGui::CollapsingHeader("Material"))
  {
    ImGui::SliderFloat("Ambient", &state.material.Ka, 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse", &state.material.Kd, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular", &state.material.Ks, 0.0f, 1.0f);
    ImGui::SliderFloat("Shininess", &state.material.Shininess, 2.0f, 1024.0f);
  }
  ImGui::End();

  ImGui::Begin("Offscreen Render");
  ImGui::BeginChild("Offscreen Render");
  auto size = ImGui::GetWindowSize();
  ImGui::Image(simgui_imtextureid(state.debug.position_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
  ImGui::Image(simgui_imtextureid(state.debug.normal_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
  ImGui::Image(simgui_imtextureid(state.debug.color_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
  ImGui::Image(simgui_imtextureid(state.debug.depth_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
  ImGui::EndChild();
  ImGui::End();
}

void frame(void)
{
  batteries::frame();

  // math required by scene
  const auto t = (float)(sapp_frame_duration() * 60.0f);
  const auto width = sapp_width();
  const auto height = sapp_height();

  // update the camera controller
  state.camera_controller.update(&state.camera, t);

  // rotate suzanne
  // state.suzanne.transform.rotation = glm::rotate(state.suzanne.transform.rotation, (float)sapp_frame_duration(), glm::vec3(0.0, 1.0, 0.0));

  // display pass matrices
  glm::vec3 light_pos = glm::vec4(0.0f, 50.0f, 0.0f, 1.0f);
  state.ambient.direction = glm::normalize(light_pos);

  // parameters for the geometry pass
  const vs_geometry_params_t vs_geometry_params = {
      .view_proj = state.camera.projection() * state.camera.view(),
  };

  // parameters for the lighting pass
  const fs_lighting_params_t fs_lighting_params = {
      .eye = state.camera.position,
      .ambient = state.ambient,
      .material = state.material,
      .lights = instance_light_data,
  };

  // render the geometry pass
  sg_begin_pass({.action = state.geometry.pass_action, .attachments = state.geometry.attachments});
  sg_apply_pipeline(state.geometry.pip);
  sg_apply_bindings(&state.geometry.bind);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_geometry_params));
  sg_draw(0, state.suzanne.mesh.num_faces * 3, state.num_instances);
  sg_end_pass();

  // render the lighting pass
  sg_begin_pass({.action = state.lighting.pass_action, .swapchain = state.lighting.attachments});
  sg_apply_pipeline(state.lighting.pip);
  sg_apply_bindings(&state.lighting.bind);
  sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_lighting_params));
  sg_draw(0, 6, 1);
  sg_end_pass();

  // sg_begin_pass({.action = state.gizmo.pass_action, .attachments = state.gizmo.attachments});
  sg_begin_pass({.action = state.gizmo.pass_action, .swapchain = sglue_swapchain()});
  sg_apply_pipeline(state.gizmo.pip);
  sg_apply_bindings(&state.gizmo.bind);
  for (auto i = 0; i < MAX_LIGHTS; i++)
  {
    state.gizmo.shape.transform.position = instance_light_data.position[i];
    // parameters for the gizmo pass
    const vs_gizmo_params_t vs_gizmo_params = {
        .view_proj = state.camera.projection() * state.camera.view(),
        .model = state.gizmo.shape.transform.matrix(),
    };

    // parameters for the gizmo pass
    const fs_gizmo_light_params_t fs_gizmo_light_params = {
        .light_color = instance_light_data.color[i],
    };
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_gizmo_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_gizmo_light_params));
    sg_draw(state.gizmo.shape.draw.base_element, state.gizmo.shape.draw.num_elements, 1);
  }

  // sg_begin_pass({.action = state.lighting.pass_action, .swapchain = state.lighting.attachments});
  // sg_apply_pipeline(state.lighting.pip);
  // sg_apply_bindings(&state.lighting.bind);
  // sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_lighting_params));
  // sg_draw(0, 6, 1);
  // sg_end_pass();

  // draw ui
  draw_ui();
  simgui_render();

  sg_end_pass();
  sg_commit();
}

void event(const sapp_event *event)
{
  batteries::event(event);
  state.camera_controller.event(event);
}

void cleanup(void)
{
  batteries::shutdown();
}

sapp_desc sokol_main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  return (sapp_desc){
      .init_cb = init,
      .frame_cb = frame,
      .event_cb = event,
      .cleanup_cb = cleanup,
      .width = 800,
      .height = 800,
      .window_title = "gpr300-sokol",
      .logger.func = slog_func,
  };
}