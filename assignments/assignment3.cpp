#define BATTERIES_IMPL
#include "batteries.h"

//
// Assignment2 -- Deferred Lighting
//

// shaders
#include "shaders/geometry_pass.h"

#include <string>

enum
{
  GEOMETRY_COLOR_COUNT = 3,
  GEOMETRY_SAMPLE_COUNT = 4,
  OFFSCREEN_WIDTH = 512,
  OFFSCREEN_HEIGHT = 512,
};

typedef struct
{
  glm::mat4 model;
  glm::mat4 view_proj;
} vs_shadow_params_t;

typedef struct
{
  glm::mat4 model;
  glm::mat4 view_proj;
} vs_display_params_t;

// application state
static struct
{
  struct
  {
    sg_pass_action pass_action;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;

    sg_bindings img;
    sg_image color_img;
    sg_image position_img;
    sg_image normal_img;
    sg_image depth_img;
  } geometry;

  struct
  {
    sg_pass_action pass_action;
  } display;

  struct
  {
    simgui_image_t color_img;
    simgui_image_t position_img;
    simgui_image_t normal_img;
    simgui_image_t depth_img;
  } debug;

  uint8_t file_buffer[batteries::megabytes(4)];

  batteries::model_t suzanne;
  glm::vec3 ambient_light;

} state = {
    .ambient_light = glm::vec3(0.25f, 0.45f, 0.65f),
};

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

void create_geometry_pass(void)
{
  const auto width = sapp_width();
  const auto height = sapp_height();

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
  auto color_smplr = sg_make_sampler((sg_sampler_desc){
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

  state.geometry.pass = sg_make_pass((sg_pass_desc){
      .color_attachments = {
          [0].image = state.geometry.position_img,
          [1].image = state.geometry.normal_img,
          [2].image = state.geometry.color_img,
      },
      .depth_stencil_attachment.image = state.geometry.depth_img,
      .label = "geometry-pass",
  });

  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = geometry_pass_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_display_params_t),
              .uniforms = {
                  [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                  [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
              },
          },
      },
      .fs = {
          .source = geometry_pass_fs,
      },
  };

  state.geometry.pip = sg_make_pipeline((sg_pipeline_desc){
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

  state.geometry.bind = (sg_bindings){
      .vertex_buffers[0] = state.suzanne.mesh.vbuf,
  };

  // create an sokol-imgui wrapper for the shadow map
  auto dbg_smp = sg_make_sampler((sg_sampler_desc){
      .min_filter = SG_FILTER_NEAREST,
      .mag_filter = SG_FILTER_NEAREST,
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .label = "ui-sampler",
  });

  state.debug.color_img = simgui_make_image((simgui_image_desc_t){
      .image = state.geometry.color_img,
      .sampler = dbg_smp,
  });
  state.debug.position_img = simgui_make_image((simgui_image_desc_t){
      .image = state.geometry.position_img,
      .sampler = dbg_smp,
  });
  state.debug.normal_img = simgui_make_image((simgui_image_desc_t){
      .image = state.geometry.normal_img,
      .sampler = dbg_smp,
  });
  state.debug.depth_img = simgui_make_image((simgui_image_desc_t){
      .image = state.geometry.depth_img,
      .sampler = dbg_smp,
  });
}

void create_display_pass(void)
{
  state.display.pass_action = {
      .colors[0] = {
          .load_action = SG_LOADACTION_CLEAR,
          .clear_value = {0.25f, 0.5f, 0.75f, 1.0f},
      },
  };
}

void init(void)
{
  batteries::setup();

  load_suzanne();
  create_geometry_pass();
  create_display_pass();
}

void draw_ui(void)
{
  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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

  const float t = (float)(sapp_frame_duration() * 60.0);
  const auto width = sapp_width();
  const auto height = sapp_height();

  // rotate suzanne
  // state.suzanne.transform.rotation = glm::rotate(state.suzanne.transform.rotation, (float)sapp_frame_duration(), glm::vec3(0.0, 1.0, 0.0));

  // display pass matrices
  glm::vec3 eye = glm::vec3(0.0f, 1.5f, 6.0f);
  glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)(width / (float)height), 0.01f, 100.0f);
  glm::mat4 view_proj = proj * view;

  // parameters for drawing suzanne
  const vs_display_params_t suzanne_vs_display_params = {
      .model = state.suzanne.transform.matrix(),
      .view_proj = view_proj,
  };

  // render the geometry pass
  sg_begin_pass(state.geometry.pass, &state.geometry.pass_action);
  sg_apply_pipeline(state.geometry.pip);
  sg_apply_bindings(&state.geometry.bind);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(suzanne_vs_display_params));
  sg_draw(0, state.suzanne.mesh.num_faces * 3, 1);
  sg_end_pass();

  sg_begin_default_pass(&state.display.pass_action, width, height);

  // draw ui
  draw_ui();
  simgui_render();

  sg_end_pass();
  sg_commit();
}

void event(const sapp_event *event)
{
  batteries::event(event);
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