#define BATTERIES_IMPL
#include "batteries.h"

//
// Work Session 3 -- Super Mario Sunshine Water
//

// shaders
#include "shaders/mariosunshine_water.h"

typedef struct
{
  glm::mat4 view_proj;
  glm::mat4 model;
} vs_water_params_t;

// application state
static struct
{
  uint8_t file_buffer[batteries::megabytes(5)];

  struct
  {
    sg_pass_action pass_action;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;

    batteries::shape_t plane;

    // texture
    sg_image img;
    sg_sampler smp[12];
    struct
    {
      uint32_t mip0[65536]; // 256x256
      uint32_t mip1[16384]; // 128x128
      uint32_t mip2[4096];  // 64*64
      uint32_t mip3[1024];  // 32*32
      uint32_t mip4[256];   // 16*16
      uint32_t mip5[64];    // 8*8
      uint32_t mip6[16];    // 4*4
      uint32_t mip7[4];     // 2*2
      uint32_t mip8[1];     // 1*2
    } pixels;
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
};

static const uint32_t mip_colors[5] = {
    0xFFFFFF00, // cyan
    0xFF00FFFF, // yellow
    0xFFFF00A0, // violet
    0xFFFFA0FF, // orange
    0xFFA000FF, // purple
};

void create_water_pass(void)
{
  // create image with mipmap content, different colors and checkboard pattern
  // we need to generate 5 mipmaps.
  // 0. black texture (256x256) [generated]
  // 1. ripple texture (128x128) [loaded]
  // 2. highlights texture (64x64) [loaded]
  // 3. ripple texture (32x32) [loaded]
  // 4. black texture (16x16) [generated]

  sg_image_data img_data;
  // auto *ptr = state.water.pixels.mip0;
  // for (int mip_index = 0; mip_index <= 8; mip_index++)
  // {
  //   const int dim = 1 << (8 - mip_index);
  //   img_data.subimage[0][mip_index].ptr = ptr;
  //   img_data.subimage[0][mip_index].size = (size_t)(dim * dim * 4);
  //   for (int y = 0; y < dim; y++)
  //   {
  //     for (int x = 0; x < dim; x++)
  //     {
  //       *ptr++ = mip_colors[mip_index];
  //     }
  //   }
  // }
  // state.water.img = sg_alloc_image();
  state.water.img = sg_make_image({
      .width = 256,
      .height = 256,
      .num_mipmaps = 1,
      .pixel_format = SG_PIXELFORMAT_RGBA8,
      .data = img_data,
  });
  auto smp = sg_make_sampler({
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
      .mipmap_filter = SG_FILTER_LINEAR,
      .wrap_u = SG_WRAP_REPEAT,
      .wrap_v = SG_WRAP_REPEAT,
      .label = "water-sampler",
  });

  state.water.pass_action = (sg_pass_action){
      .colors[0] = {
          .clear_value = {state.scene.ambient_light.r, state.scene.ambient_light.g, state.scene.ambient_light.b, 1.0f},
          .load_action = SG_LOADACTION_CLEAR,
      },
  };

  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = mariosunshine_water_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_water_params_t),
              .uniforms = {
                  [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                  [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
              },
          },
      },
      .fs = {
          .source = mariosunshine_water_fs,
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
      .cull_mode = SG_CULLMODE_BACK,
      .depth = {
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .label = "water-pipeline",
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
}

void init(void)
{
  batteries::setup();
  create_water_pass();
}

void frame(void)
{
  batteries::frame();

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
  ImGui::End();

  const auto width = sapp_width();
  const auto height = sapp_height();

  // math required by the shader
  auto camera_view_proj = state.camera.projection() * state.camera.view();

  // initialize uniform data
  const vs_water_params_t vs_params = {
      .view_proj = camera_view_proj,
      .model = state.water.plane.transform.matrix(),
  };

  // graphics pass
  sg_begin_pass({.action = state.water.pass_action, .swapchain = sglue_swapchain()});
  // sg_apply_pipeline(state.water.pip);
  // sg_apply_bindings(&state.water.bind);
  // sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
  // sg_draw(state.water.plane.draw.base_element, state.water.plane.draw.num_elements, 1);

  // draw ui
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