#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Work Session 3 -- Super Mario Sunshine Water
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"
#include "batteries/materials.h"
#include "batteries/lights.h"

// shaders
#include "shaders/mariosunshine_water.h"

typedef struct
{
  glm::mat4 model_view_proj;
} vs_params_t;

// application state
static struct
{
  uint8_t file_buffer[boilerplate::megabytes(5)];

  sg_pipeline pip;
  sg_buffer vbuf;
  sg_image img;
  sg_sampler smp[12];
  float r;
  int sidx;
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
    .sidx = 0,
    .time = {
        .frame = 0.0,
        .absolute = 0.0,
        .factor = 1.0f,
        .paused = false,
    },
};

static const uint32_t mip_colors[9] = {
    0xFF0000FF, // red
    0xFF00FF00, // green
    0xFFFF0000, // blue
    0xFFFF00FF, // magenta
    0xFFFFFF00, // cyan
    0xFF00FFFF, // yellow
    0xFFFF00A0, // violet
    0xFFFFA0FF, // orange
    0xFFA000FF, // purple
};

void create_water_texture(void)
{
  // create image with mipmap content, different colors and checkboard pattern
  // we need to generate 5 mipmaps.
  // 0. black texture (256x256) [generated]
  // 1. ripple texture (128x128) [loaded]
  // 2. highlights texture (64x64) [loaded]
  // 3. ripple texture (32x32) [loaded]
  // 4. black texture (16x16) [generated]

  // a plane vertex buffer
  float vertices[] = {
      -1.0,
      -1.0,
      0.0,
      0.0,
      0.0,
      +1.0,
      -1.0,
      0.0,
      1.0,
      0.0,
      -1.0,
      +1.0,
      0.0,
      0.0,
      1.0,
      +1.0,
      +1.0,
      0.0,
      1.0,
      1.0,
  };
  state.vbuf = sg_make_buffer((sg_buffer_desc){
      .data = SG_RANGE(vertices),
  });

  // create image with mipmap content, different colors and checkboard pattern
  sg_image_data img_data;
  uint32_t *ptr = state.pixels.mip0;
  bool even_odd = false;
  for (int mip_index = 0; mip_index <= 8; mip_index++)
  {
    const int dim = 1 << (8 - mip_index);
    img_data.subimage[0][mip_index].ptr = ptr;
    img_data.subimage[0][mip_index].size = (size_t)(dim * dim * 4);
    for (int y = 0; y < dim; y++)
    {
      for (int x = 0; x < dim; x++)
      {
        *ptr++ = mip_colors[mip_index];
      }
    }
  }
  state.img = sg_make_image((sg_image_desc){
      .width = 256,
      .height = 256,
      .num_mipmaps = 9,
      .pixel_format = SG_PIXELFORMAT_RGBA8,
      .data = img_data,
  });

  // the first 4 samplers are just different min-filters
  sg_sampler_desc smp_desc = {
      .mag_filter = SG_FILTER_LINEAR,
  };
  sg_filter filters[] = {
      SG_FILTER_NEAREST,
      SG_FILTER_LINEAR,
  };
  sg_filter mipmap_filters[] = {
      SG_FILTER_NEAREST,
      SG_FILTER_LINEAR,
  };
  int smp_index = 0;
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      smp_desc.min_filter = filters[i];
      smp_desc.mipmap_filter = mipmap_filters[j];
      smp_desc.min_lod = 0.0f;
      smp_desc.max_lod = 4.0f;
      state.smp[smp_index++] = sg_make_sampler(&smp_desc);
    }
  }
  // the next 4 samplers use min_lod/max_lod
  smp_desc.min_lod = 2.0f;
  smp_desc.max_lod = 4.0f;
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      smp_desc.min_filter = filters[i];
      smp_desc.mipmap_filter = mipmap_filters[j];
      state.smp[smp_index++] = sg_make_sampler(&smp_desc);
    }
  }
  // the last 4 samplers use different anistropy levels
  smp_desc.min_lod = 0.0f;
  smp_desc.max_lod = 0.0f; // for max_lod, zero-initialized means "FLT_MAX"
  smp_desc.min_filter = SG_FILTER_LINEAR;
  smp_desc.mag_filter = SG_FILTER_LINEAR;
  smp_desc.mipmap_filter = SG_FILTER_LINEAR;
  for (int i = 0; i < 4; i++)
  {
    smp_desc.max_anisotropy = 1 << i;
    state.smp[smp_index++] = sg_make_sampler(&smp_desc);
  }
  assert(smp_index == 12);

  // shader
  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = mariosunshine_water_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_params_t),
              .uniforms = {
                  [0] = {.name = "model_view_proj", .type = SG_UNIFORMTYPE_MAT4},
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

  // pipeline state
  state.pip = sg_make_pipeline((sg_pipeline_desc){
      .layout = {
          .attrs = {
              [0].format = SG_VERTEXFORMAT_FLOAT3,
              [1].format = SG_VERTEXFORMAT_FLOAT2,
          },
      },
      .shader = sg_make_shader(shader_desc),
      .primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
  });
}

void init(void)
{
  boilerplate::setup();
  create_water_texture();
}

void draw_ui(void)
{
  const auto t = (float)sapp_frame_duration();
  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", t * 1000, 1.0f / t, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::SliderFloat("Time Factor", &state.time.factor, 0.0f, 1.0f);
  ImGui::SliderInt("Sampler", &state.sidx, 0, 11);
  ImGui::End();
}

void frame(void)
{
  boilerplate::frame();
  draw_ui();

  const auto t = (float)sapp_frame_duration();
  state.camera_controller.update(&state.camera, t);

  state.time.frame = (float)sapp_frame_duration();
  if (!state.time.paused)
  {
    state.time.absolute += state.time.frame * state.time.factor;
  }

  // math required by the shader
  const auto camera_view_proj = state.camera.projection() * state.camera.view();

  vs_params_t vs_params;

  state.r += 0.1f * 60.0f * (float)sapp_frame_duration();
  const glm::mat4 rm = glm::rotate(state.r, glm::vec3(1.0f, 0.0f, 0.0f));

  // graphics pass
  sg_bindings bind = {
      .vertex_buffers[0] = state.vbuf,
      .fs.images[0] = state.img,
  };
  vs_params.model_view_proj = camera_view_proj * glm::scale(glm::mat4(1.0f), glm::vec3(500.0f, 500.0f, 500.0f));
  bind.fs.samplers[0] = state.smp[state.sidx];

  sg_begin_pass((sg_pass){.swapchain = sglue_swapchain()});
  sg_apply_pipeline(state.pip);
  sg_apply_bindings(&bind);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
  sg_draw(0, 4, 1);

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