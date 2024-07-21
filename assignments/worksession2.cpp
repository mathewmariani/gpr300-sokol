#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Work Session 1 -- Landmass Generation
//

#include "batteries/camera.h"

// shaders
#include "shaders/island_generator.h"

typedef struct
{
  glm::mat4 view_proj;
  glm::mat4 model;
  glm::vec3 camera_pos;
  float scale;
} vs_water_params_t;

// application state
static struct
{
  uint8_t file_buffer[boilerplate::megabytes(5)];

  struct
  {
    sg_pass_action pass_action;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;
    sg_image img;

    boilerplate::shape_t plane;
  } landmass;

  struct
  {
    double frame;
    double absolute;
    float factor;
    bool paused;
  } time;

  struct
  {
    float scale;
  } demo;

  struct
  {
    glm::vec3 ambient_light;
  } scene;

  batteries::camera_t camera;
  batteries::camera_controller_t camera_controller;
} state = {
    .time = {
        .frame = 0.0,
        .absolute = 0.0,
        .factor = 1.0f,
        .paused = false,
    },
    .demo = {
        .scale = 10.0f,
    },
};

void create_water_pass(void)
{
  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = island_generator_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_water_params_t),
              .uniforms = {
                  [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                  [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                  [2] = {.name = "cameraPos", .type = SG_UNIFORMTYPE_FLOAT3},
                  [3] = {.name = "landmass.scale", .type = SG_UNIFORMTYPE_FLOAT},
              },
          },
          .images = {
              [0] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
          },
          .samplers = {
              [0] = {.used = true, .sampler_type = SG_SAMPLERTYPE_FILTERING},
          },
          .image_sampler_pairs = {
              [0] = {
                  .used = true,
                  .glsl_name = "heightmap",
                  .image_slot = 0,
                  .sampler_slot = 0,
              },
          },
      },
      .fs = {
          .source = island_generator_fs,
          .images = {
              [0] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
          },
          .samplers = {
              [0] = {.used = true, .sampler_type = SG_SAMPLERTYPE_FILTERING},
          },
          .image_sampler_pairs = {
              [0] = {
                  .used = true,
                  .glsl_name = "heightmap",
                  .image_slot = 0,
                  .sampler_slot = 0,
              },
          },
      },
  };

  state.landmass.pass_action = (sg_pass_action){
      .colors[0] = {
          .clear_value = {state.scene.ambient_light.r, state.scene.ambient_light.g, state.scene.ambient_light.b, 1.0f},
          .load_action = SG_LOADACTION_CLEAR,
      }};

  state.landmass.pip = sg_make_pipeline({
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
      .label = "landmass-pipeline",
  });

  state.landmass.img = sg_alloc_image();
  sg_sampler smp = sg_make_sampler({
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
      .wrap_u = SG_WRAP_REPEAT,
      .wrap_v = SG_WRAP_REPEAT,
      .label = "heightmap-sampler",
  });

  // generate shape geometries
  auto sizes = sshape_plane_sizes(100);
  std::vector<sshape_vertex_t> vertices(sizes.vertices.num);
  std::vector<uint16_t> indices(sizes.indices.num);

  sshape_buffer_t buf = {
      .vertices.buffer = {.ptr = vertices.data(), .size = sizes.vertices.size},
      .indices.buffer = {.ptr = indices.data(), .size = sizes.indices.size},
  };

  sshape_plane_t plane = {
      .width = 100.0f,
      .depth = 100.0f,
      .tiles = 100,
  };

  buf = sshape_build_plane(&buf, &plane);
  state.landmass.plane.draw = sshape_element_range(&buf);

  state.landmass.plane.transform.position = glm::vec3(0.0f, -1.0f, 0.0f);

  // one vertex/index-buffer-pair for all shapes
  const auto vbuf_desc = sshape_vertex_buffer_desc(&buf);
  const auto ibuf_desc = sshape_index_buffer_desc(&buf);

  state.landmass.bind = (sg_bindings){
      .vs = {
          .images[0] = state.landmass.img,
          .samplers[0] = smp,
      },
      .fs = {
          .images[0] = state.landmass.img,
          .samplers[0] = smp,
      },
      .vertex_buffers[0] = sg_make_buffer(&vbuf_desc),
      .index_buffer = sg_make_buffer(&ibuf_desc),
  };

  boilerplate::assets::load_img({
      .image_id = state.landmass.img,
      .path = "assets/materials/heightmap.png",
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

  if (ImGui::CollapsingHeader("Landmass"))
  {
    ImGui::SliderFloat("Scale", &state.demo.scale, 1.0f, 100.0f);
  }
  ImGui::End();

  const auto width = sapp_width();
  const auto height = sapp_height();

  // math required by the shader
  auto camera_view_proj = state.camera.projection() * state.camera.view();

  // initialize uniform data
  const vs_water_params_t vs_params = {
      .view_proj = camera_view_proj,
      .model = state.landmass.plane.transform.matrix(),
      .camera_pos = state.camera.position,
      .scale = state.demo.scale,
  };

  // graphics pass
  sg_begin_pass({.action = state.landmass.pass_action, .swapchain = sglue_swapchain()});
  sg_apply_pipeline(state.landmass.pip);
  sg_apply_bindings(&state.landmass.bind);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
  sg_draw(state.landmass.plane.draw.base_element, state.landmass.plane.draw.num_elements, 1);

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