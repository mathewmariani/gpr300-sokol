#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Work Session 1 -- Windwaker Ocean
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"
#include "batteries/shape.h"
#include "batteries/materials.h"
#include "batteries/lights.h"

// shaders
#include "batteries/shaders/windwaker_water.h"

typedef struct
{
  glm::mat4 view_proj;
  glm::mat4 model;
  glm::vec3 camera_pos;
  float scale;
  float strength;
  float tiling;
  float time;
  glm::vec3 color;
  glm::vec2 direction;
  float top_scale;
  float bottom_scale;
} vs_water_params_t;

// application state
static struct
{
  struct
  {
    sg_pass_action pass_action;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;
    sg_image img;

    batteries::shape_t plane;
  } water;

  struct
  {
    double frame;
    double absolute;
    float factor;
    bool paused;
  } time;

  struct
  {
    glm::vec3 color;
    glm::vec2 direction;
    float scale;
    float strength;
    float tiling;
    float top_scale;
    float bottom_scale;
  } ocean;

  struct
  {
    glm::vec3 ambient_light;
  } scene;

  batteries::camera_t camera;
  batteries::camera_controller_t camera_controller;

  uint8_t file_buffer[boilerplate::megabytes(5)];
} state = {
    .time = {
        .frame = 0.0,
        .absolute = 0.0,
        .factor = 1.0f,
        .paused = false,
    },
    .ocean = {
        .color = glm::vec3(0.00f, 0.31f, 0.85f),
        .direction = glm::vec3(0.5f),
        .scale = 10.0f,
        .strength = 1.0f,
        .tiling = 10.0f,
        .top_scale = 0.90f,
        .bottom_scale = 0.02f,
    },
};

void create_water_pass(void)
{
  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = windwaker_water_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_water_params_t),
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
  };

  state.water.pass_action = (sg_pass_action){
      .colors[0] = {
          .clear_value = {state.scene.ambient_light.r, state.scene.ambient_light.g, state.scene.ambient_light.b, 1.0f},
          .load_action = SG_LOADACTION_CLEAR,
      }};

  state.water.pip = sg_make_pipeline({
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
      .depth = {
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .label = "water-pipeline",
  });

  state.water.img = sg_alloc_image();
  sg_sampler smp = sg_make_sampler({
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
      .wrap_u = SG_WRAP_REPEAT,
      .wrap_v = SG_WRAP_REPEAT,
      .label = "water-sampler",
  });

  // generate shape geometries
  sshape_vertex_t vertices[2 * 1024];
  uint16_t indices[4 * 1024];
  sshape_buffer_t buf = {
      .vertices.buffer = SSHAPE_RANGE(vertices),
      .indices.buffer = SSHAPE_RANGE(indices),
  };

  sshape_plane_t plane = {
      .width = 100.0f,
      .depth = 100.0f,
      .tiles = 10,
  };
  buf = sshape_build_plane(&buf, &plane);
  state.water.plane.draw = sshape_element_range(&buf);

  state.water.plane.transform.position = glm::vec3(0.0f, -1.0f, 0.0f);

  // one vertex/index-buffer-pair for all shapes
  const auto vbuf_desc = sshape_vertex_buffer_desc(&buf);
  const auto ibuf_desc = sshape_index_buffer_desc(&buf);

  state.water.bind = (sg_bindings){
      .vertex_buffers[0] = sg_make_buffer(&vbuf_desc),
      .index_buffer = sg_make_buffer(&ibuf_desc),
      .fs = {
          .images[0] = state.water.img,
          .samplers[0] = smp,
      },
  };

  batteries::load_img({
      .image_id = state.water.img,
      .path = "assets/materials/water.png",
      .buffer = SG_RANGE(state.file_buffer),
  });
}

void init(void)
{
  boilerplate::setup();
  create_water_pass();
}

void frame(void)
{
  boilerplate::frame();

  const auto t = (float)sapp_frame_duration();
  state.camera_controller.update(&state.camera, t);

  state.time.frame = (float)sapp_frame_duration();
  if (!state.time.paused)
  {
    state.time.absolute += state.time.frame * state.time.factor;
  }

  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", t * 1000, 1.0f / t, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  ImGui::SliderFloat("Time Factor", &state.time.factor, 0.0f, 1.0f);

  if (ImGui::CollapsingHeader("Color"))
  {
    ImGui::ColorEdit3("Color", &state.ocean.color[0]);
    ImGui::SliderFloat("Upper Blend", &state.ocean.top_scale, 0.0f, 1.0f);
    ImGui::SliderFloat("Bottom Blend", &state.ocean.bottom_scale, 0.0f, 1.0f);
  }
  if (ImGui::CollapsingHeader("Texture"))
  {
    ImGui::SliderFloat("Tilling", &state.ocean.tiling, 1.0f, 10.0f);
    ImGui::SliderFloat2("Scroll Direction", &state.ocean.direction[0], -1.0f, 1.0f);
  }
  if (ImGui::CollapsingHeader("Wave"))
  {
    ImGui::SliderFloat("Scale", &state.ocean.scale, 1.0f, 100.0f);
    ImGui::SliderFloat("Strength", &state.ocean.strength, 1.0f, 100.0f);
  }
  ImGui::End();

  const auto width = sapp_width();
  const auto height = sapp_height();

  // math required by the shader
  auto camera_view_proj = state.camera.projection() * state.camera.view();

  // initialize uniform data
  const vs_water_params_t vs_params = {
      .view_proj = camera_view_proj,
      .model = state.water.plane.transform.matrix(),
      .camera_pos = state.camera.position,
      .scale = state.ocean.scale,
      .strength = state.ocean.strength,
      .time = (float)state.time.absolute,
      .color = state.ocean.color,
      .direction = state.ocean.direction,
      .tiling = state.ocean.tiling,
      .top_scale = state.ocean.top_scale,
      .bottom_scale = state.ocean.bottom_scale,
  };

  // graphics pass
  sg_begin_pass({.action = state.water.pass_action, .swapchain = sglue_swapchain()});
  sg_apply_pipeline(state.water.pip);
  sg_apply_bindings(&state.water.bind);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
  sg_draw(state.water.plane.draw.base_element, state.water.plane.draw.num_elements, 1);

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