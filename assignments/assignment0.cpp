#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Assignment0 -- Blinn Phong
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"

// shaders
#include "shaders/blinn_phong.h"
#include "shaders/no_post_process.h"
#include "shaders/shapes.h"

// stl
#include <unordered_map>

typedef struct
{
  float brightness;
  glm::vec3 color;
  glm::vec3 position;
} light_t;

typedef struct
{
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;
} material_t;

typedef struct
{
  glm::mat4 view_proj;
  glm::mat4 model;
} vs_blinnphong_params_t;

typedef struct
{
  material_t material;
  light_t light;
  glm::vec3 camera_position;
} fs_blinnphong_params_t;

typedef struct
{
  glm::mat4 view_proj;
  glm::mat4 model;
} vs_gizmo_params_t;

typedef struct
{
  glm::vec3 light_color;
} fs_gizmo_light_params_t;

// http://devernay.free.fr/cours/opengl/materials.html
static std::unordered_map<std::string, material_t> material_map = {
    {"emerald", {{0.0215f, 0.1745f, 0.0215f}, {0.07568f, 0.61424f, 0.07568f}, {0.633f, 0.727811f, 0.633f}, 0.6f}},
    {"jade", {{0.135f, 0.2225f, 0.1575f}, {0.54f, 0.89f, 0.63f}, {0.316228f, 0.316228f, 0.316228f}, 0.1}},
    {"obsidian", {{0.05375f, 0.05f, 0.06625f}, {0.18275f, 0.17f, 0.22525f}, {0.332741f, 0.328634f, 0.346435f}, 0.3f}},
    {"pearl", {{0.25f, 0.20725f, 0.20725f}, {1.0f, 0.829f, 0.829f}, {0.296648f, 0.296648f, 0.296648f}, 0.088f}},
    {"ruby", {{0.1745f, 0.01175f, 0.01175f}, {0.61424f, 0.04136f, 0.04136f}, {0.727811f, 0.626959f, 0.626959f}, 0.6f}},
    {"turquoise", {{0.1f, 0.18725f, 0.1745f}, {0.396f, 0.74151f, 0.69102f}, {0.297254f, 0.30829f, 0.306678f}, 0.1f}},
    {"brass", {{0.329412f, 0.223529f, 0.027451f}, {0.780392f, 0.568627f, 0.113725f}, {0.992157f, 0.941176f, 0.807843f}, 0.21794872f}},
    {"bronze", {{0.2125f, 0.1275f, 0.054f}, {0.714f, 0.4284f, 0.18144f}, {0.393548f, 0.271906f, 0.166721f}, 0.2f}},
    {"chrome", {{0.25f, 0.25f, 0.25f}, {0.4f, 0.4f, 0.4f}, {0.774597f, 0.774597f, 0.774597f}, 0.6f}},
    {"copper", {{0.19125f, 0.0735f, 0.0225f}, {0.7038f, 0.27048f, 0.0828f}, {0.256777f, 0.137622f, 0.086014f}, 0.1f}},
    {"gold", {{0.24725f, 0.1995f, 0.0745f}, {0.75164f, 0.60648f, 0.22648f}, {0.628281f, 0.555802f, 0.366065f}, 0.4f}},
    {"silver", {{0.19225f, 0.19225f, 0.19225f}, {0.50754f, 0.50754f, 0.50754f}, {0.508273f, 0.508273f, 0.508273f}, 0.4f}},
    {"black plastic", {{0.0f, 0.0f, 0.0f}, {0.01f, 0.01f, 0.01f}, {0.50f, 0.50f, 0.50f}, 0.25f}},
    {"cyan plastic", {{0.0f, 0.1f, 0.06f}, {0.0f, 0.50980392f, 0.50980392f}, {0.50196078f, 0.50196078f, 0.50196078f}, 0.25f}},
    {"green plastic", {{0.0f, 0.0f, 0.0f}, {0.1f, 0.35f, 0.1f}, {0.45f, 0.55f, 0.45f}, 0.25f}},
    {"red plastic", {{0.0f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, {0.7f, 0.6f, 0.6f}, 0.25f}},
    {"white plastic", {{0.0f, 0.0f, 0.0f}, {0.55f, 0.55f, 0.55f}, {0.70f, 0.70f, 0.70f}, 0.25f}},
    {"yellow plastic", {{0.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.0f}, {0.60f, 0.60f, 0.50f}, 0.25f}},
    {"black rubber", {{0.02f, 0.02f, 0.02f}, {0.01f, 0.01f, 0.01f}, {0.4f, 0.4f, 0.4f}, 0.078125f}},
    {"cyan rubber", {{0.0f, 0.05f, 0.05f}, {0.4f, 0.5f, 0.5f}, {0.04f, 0.7f, 0.7f}, 0.078125f}},
    {"green rubber", {{0.0f, 0.05f, 0.0f}, {0.4f, 0.5f, 0.4f}, {0.04f, 0.7f, 0.04f}, 0.078125f}},
    {"red rubber", {{0.05f, 0.0f, 0.0f}, {0.5f, 0.4f, 0.4f}, {0.7f, 0.04f, 0.04f}, 0.078125f}},
    {"white rubber", {{0.05f, 0.05f, 0.05f}, {0.5f, 0.5f, 0.5f}, {0.7f, 0.7f, 0.7f}, .078125f}},
    {"yellow rubber", {{0.05f, 0.05f, 0.0f}, {0.5f, 0.5f, 0.4f}, {0.7f, 0.7f, 0.04f}, .078125f}},
};

// application state
static struct
{
  uint8_t file_buffer[boilerplate::megabytes(5)];

  struct
  {
    sg_attachments attachments;
    sg_image color;
    sg_image depth;
    sg_sampler sampler;
  } framebuffer;

  struct
  {
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
  } display;

  struct
  {
    sg_attachments attachments; // FIXME: not needed
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
  } blinnphong;

  struct
  {
    sg_attachments attachments; // FIXME: not needed
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
    sshape_element_range_t sphere;
  } gizmo;

  batteries::camera_t camera;
  batteries::camera_controller_t camera_controller;
  light_t light;

  struct
  {
    float ry;
    batteries::model_t suzanne;
    material_t material;
  } scene;
} state = {
    .light = {
        .brightness = 1.0f,
        .color = glm::vec3(0.25f, 0.45f, 0.65f),
    },
    .scene = {
        .ry = 0.0f,
    },
};

void load_suzanne(void)
{
  state.scene.suzanne.mesh.vbuf = sg_alloc_buffer();
  batteries::load_obj({
      .buffer_id = state.scene.suzanne.mesh.vbuf,
      .mesh = &state.scene.suzanne.mesh,
      .path = "assets/suzanne.obj",
      .buffer = SG_RANGE(state.file_buffer),
  });
}

// NOTE: not required by the assignment
void create_framebuffer(void)
{
  const auto width = sapp_width();
  const auto height = sapp_height();

  // color attachment
  state.framebuffer.color = sg_make_image({
      .pixel_format = SG_PIXELFORMAT_RGBA8,
      .render_target = true,
      .width = width,
      .height = height,
      .label = "color-image",
  });

  // depth attachment
  state.framebuffer.depth = sg_make_image({
      .pixel_format = SG_PIXELFORMAT_DEPTH,
      .render_target = true,
      .width = width,
      .height = height,
      .label = "depth-image",
  });

  // image sampler
  state.framebuffer.sampler = sg_make_sampler({
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
  });
}

void create_display_pass()
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

  auto quad_buffer = sg_make_buffer({
      .data = SG_RANGE(quad_vertices),
      .label = "quad-vertices",
  });

  auto display_shader_desc = (sg_shader_desc){
      .vs = {
          .source = no_post_process_vs,
      },
      .fs = {
          .source = no_post_process_fs,
          .images[0].used = true,
          .samplers[0].used = true,
          .image_sampler_pairs[0] = {
              .glsl_name = "screen",
              .image_slot = 0,
              .sampler_slot = 0,
              .used = true,
          },
      },
  };

  state.display.pass_action = (sg_pass_action){
      .colors[0].load_action = SG_LOADACTION_CLEAR,
      .depth.load_action = SG_LOADACTION_DONTCARE,
      .stencil.load_action = SG_LOADACTION_DONTCARE,
  };

  state.display.pip = sg_make_pipeline({
      .layout = {
          .attrs = {
              [0].format = SG_VERTEXFORMAT_FLOAT2,
              [1].format = SG_VERTEXFORMAT_FLOAT2,
          },
      },
      .shader = sg_make_shader(display_shader_desc),
      .label = "display-pipeline",
  });

  // apply bindings
  state.display.bind = (sg_bindings){
      .vertex_buffers[0] = quad_buffer,
      .fs = {
          .images[0] = state.framebuffer.color,
          .samplers[0] = state.framebuffer.sampler,
      },
  };
}

void create_gizmo_pass(void)
{
  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = shapes_vs,
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
          .source = shapes_fs,
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
      .colors[0].load_action = SG_LOADACTION_LOAD,
      .depth.load_action = SG_LOADACTION_LOAD,
  };

  state.gizmo.attachments = sg_make_attachments((sg_attachments_desc){
      .colors[0].image = state.framebuffer.color,
      .depth_stencil.image = state.framebuffer.depth,
      .label = "gizmo-attachment",
  });

  state.gizmo.pip = sg_make_pipeline({
      .shader = sg_make_shader(shader_desc),
      .layout = {
          .buffers[0] = sshape_vertex_buffer_layout_state(),
          .attrs = {
              [0] = sshape_position_vertex_attr_state(),
              [1] = sshape_normal_vertex_attr_state(),
              [2] = sshape_texcoord_vertex_attr_state(),
              [3] = sshape_color_vertex_attr_state(),
          },
      },
      .index_type = SG_INDEXTYPE_UINT16,
      .cull_mode = SG_CULLMODE_NONE,
      .depth = {
          .pixel_format = SG_PIXELFORMAT_DEPTH,
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .label = "gizmo-pipeline",
  });

  // generate shape geometries
  sshape_vertex_t vertices[30] = {0}; // (slices + 1) * (stacks + 1);
  uint16_t indices[90] = {0};         // ((2 * slices * stacks) - (2 * slices)) * 3;
  sshape_buffer_t buf = {
      .vertices.buffer = SSHAPE_RANGE(vertices),
      .indices.buffer = SSHAPE_RANGE(indices),
  };
  const sshape_sphere_t sphere = {
      .radius = 0.125f,
      .slices = 5,
      .stacks = 4,
  };
  buf = sshape_build_sphere(&buf, &sphere);
  assert(buf.valid);

  // one vertex/index-buffer-pair for all shapes
  state.gizmo.sphere = sshape_element_range(&buf);
  const sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&buf);
  const sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&buf);
  state.gizmo.bind = (sg_bindings){
      .vertex_buffers[0] = sg_make_buffer(&vbuf_desc),
      .index_buffer = sg_make_buffer(&ibuf_desc),
  };
}

void create_blinnphong_pass(void)
{
  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = blinn_phong_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_blinnphong_params_t),
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
              .size = sizeof(fs_blinnphong_params_t),
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

  state.blinnphong.pass_action = (sg_pass_action){
      .colors[0] = {
          .clear_value = {
              state.light.color.r * state.light.brightness,
              state.light.color.g * state.light.brightness,
              state.light.color.b * state.light.brightness,
              1.0f,
          },
          .load_action = SG_LOADACTION_CLEAR,
      },
  };

  state.blinnphong.attachments = sg_make_attachments({
      .colors[0].image = state.framebuffer.color,
      .depth_stencil.image = state.framebuffer.depth,
  });

  state.blinnphong.pip = sg_make_pipeline({
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

  state.blinnphong.bind = (sg_bindings){
      .vertex_buffers[0] = state.scene.suzanne.mesh.vbuf,
  };
}

void init(void)
{
  boilerplate::setup();

  load_suzanne();
  create_framebuffer();
  create_display_pass();
  create_gizmo_pass();
  create_blinnphong_pass();

  state.scene.material = material_map.at("emerald");
}

static void update_clear_color(void)
{
  state.blinnphong.pass_action.colors[0].clear_value = {
      0.0f,
      0.0f,
      0.0f,
      1.0f,
  };
}

void draw_ui(void)
{
  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  if (ImGui::CollapsingHeader("Light"))
  {
    if (ImGui::SliderFloat("Brightness", &state.light.brightness, 0.0f, 1.0f))
    {
      // update_clear_color();
    }
    if (ImGui::ColorEdit3("Color", &state.light.color[0]))
    {
      // update_clear_color();
    }
  }

  static int current_item = 0; // Index for currently selected item
  int item_index = 0;          // Used to track the index of the current item in the loop
  std::string selected_material_name;

  if (ImGui::BeginCombo("Material", material_map.begin()->first.c_str()))
  {
    for (const auto &pair : material_map)
    {
      bool is_selected = (current_item == item_index);
      if (ImGui::Selectable(pair.first.c_str(), is_selected))
      {
        current_item = item_index;
        state.scene.material = pair.second;
      }
      if (is_selected)
      {
        ImGui::SetItemDefaultFocus();
      }
      item_index++;
    }
    ImGui::EndCombo();
  }
  ImGui::End();
}

void frame(void)
{
  boilerplate::frame();
  draw_ui();

  const auto t = (float)sapp_frame_duration();
  state.camera_controller.update(&state.camera, t);
  state.scene.ry += 0.2f * t;

  // sugar: rotate light
  const auto rym = glm::rotate(state.scene.ry, glm::vec3(0.0f, 1.0f, 0.0f));
  state.light.position = rym * glm::vec4(5.0f, 0.0f, 5.0f, 1.0f);

  const auto view_proj = state.camera.projection() * state.camera.view();

  // initialize uniform data
  const vs_blinnphong_params_t vs_params = {
      .view_proj = view_proj,
      .model = state.scene.suzanne.transform.matrix(),
  };
  const fs_blinnphong_params_t fs_params = {
      .material = state.scene.material,
      .light = state.light,
      .camera_position = state.camera.position,
  };
  const vs_gizmo_params_t vs_gizmo_params = {
      .view_proj = view_proj,
      .model = glm::translate(glm::mat4(1.0f), state.light.position),
  };
  const fs_gizmo_light_params_t fs_gizmo_light_params = {
      .light_color = state.light.color,
  };

  // NOTE: blinn-phong pass.
  sg_begin_pass({.action = state.blinnphong.pass_action, .attachments = state.blinnphong.attachments});
  sg_apply_pipeline(state.blinnphong.pip);
  sg_apply_bindings(&state.blinnphong.bind);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
  sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
  sg_draw(0, state.scene.suzanne.mesh.num_faces * 3, 1);
  sg_end_pass();

  // render light sources.
  sg_begin_pass({.action = state.gizmo.pass_action, .attachments = state.gizmo.attachments});
  sg_apply_pipeline(state.gizmo.pip);
  sg_apply_bindings(&state.gizmo.bind);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_gizmo_params));
  sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_gizmo_light_params));
  sg_draw(state.gizmo.sphere.base_element, state.gizmo.sphere.num_elements, 1);
  sg_end_pass();

  // display pass
  sg_begin_pass({.action = state.display.pass_action, .swapchain = sglue_swapchain()});
  sg_apply_pipeline(state.display.pip);
  sg_apply_bindings(&state.display.bind);
  sg_draw(0, 6, 1);

  // draw ui
  simgui_render();

  sg_end_pass();
  sg_commit();
}

void event(const sapp_event *event)
{
  boilerplate::event(event);
  state.camera_controller.event(event);
}

void cleanup(void)
{
  boilerplate::shutdown();
}