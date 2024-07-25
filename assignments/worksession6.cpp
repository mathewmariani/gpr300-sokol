#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Work Session 4 -- Toonshader
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"
#include "batteries/materials.h"
#include "batteries/lights.h"

// shaders
#include "shaders/windwaker_object.h"
#include "shaders/windwaker_sea.h"

typedef struct
{
  glm::vec3 highlight;
  glm::vec3 shadow;
} palette_t;

typedef struct
{
  float time;
  glm::vec3 color;
} water_properties_t;

typedef struct
{
  glm::mat4 view_proj;
  glm::mat4 model;
} vs_world_params_t;

typedef struct
{
  batteries::ambient_t ambient;
  palette_t palette;
} fs_world_params_t;

typedef struct
{
  batteries::ambient_t ambient;
  palette_t palette;
  water_properties_t water;
} fs_water_params_t;

// application state
static struct
{
  uint8_t file_buffer[boilerplate::megabytes(5)];

  palette_t pal[4];

  struct
  {
    sg_pass_action pass_action;
    sg_pass pass;
  } windwaker;

  struct
  {
    sg_pipeline pip;
    sg_bindings bind;
  } world_pass;

  struct
  {
    sg_pipeline pip;
    sg_bindings bind;
  } water_pass;

  struct
  {
    double frame;
    double absolute;
    float factor;
    bool paused;
  } time;

  struct
  {
    float ry;
    batteries::ambient_t ambient;
    palette_t palette;

    struct
    {
      batteries::model_t model;
      std::vector<sg_image> materials;
    } island;

    struct
    {
      batteries::model_t model;
      std::vector<sg_image> materials;
    } water;
  } scene;

  batteries::camera_t camera;
  batteries::camera_controller_t camera_controller;
} state = {
    .pal = {
        {.highlight = glm::vec3(1.00f, 1.00f, 1.00f), .shadow = glm::vec3(0.60f, 0.54f, 0.52f)},
        {.highlight = glm::vec3(0.47f, 0.58f, 0.68f), .shadow = glm::vec3(0.32f, 0.39f, 0.57f)},
        {.highlight = glm::vec3(0.62f, 0.69f, 0.67f), .shadow = glm::vec3(0.50f, 0.55f, 0.50f)},
        {.highlight = glm::vec3(0.24f, 0.36f, 0.54f), .shadow = glm::vec3(0.25f, 0.31f, 0.31f)},
    },
    .camera = {
        .position = glm::vec3(115.0f, 75.0f, 850.0f),
    },
    .time = {
        .frame = 0.0,
        .absolute = 0.0,
        .factor = 1.0f,
        .paused = false,
    },
    .scene = {
        .ry = 0.0f,
        .ambient = {
            .color = glm::vec3(0.25f, 0.45f, 0.65f),
        },
        .palette = {
            .highlight = glm::vec3(1.00f, 1.00f, 1.00f),
            .shadow = glm::vec3(0.60f, 0.54f, 0.52f),
        },
    },
};

void load_island(void)
{
  state.scene.island.model.mesh.vbuf = sg_alloc_buffer();
  batteries::load_obj({
      .buffer_id = state.scene.island.model.mesh.vbuf,
      .mesh = &state.scene.island.model.mesh,
      .path = "assets/island/Island.obj",
      .buffer = SG_RANGE(state.file_buffer),
  });

  state.scene.water.model.mesh.vbuf = sg_alloc_buffer();
  batteries::load_obj({
      .buffer_id = state.scene.water.model.mesh.vbuf,
      .mesh = &state.scene.water.model.mesh,
      .path = "assets/island/Sea.obj",
      .buffer = SG_RANGE(state.file_buffer),
  });

  state.scene.island.materials.reserve(8);
  state.scene.water.materials.reserve(8);

#define LOAD_ISLAND_IMAGE(i, filepath)                                      \
  state.scene.island.materials[i] = sg_alloc_image();                       \
  batteries::load_img({.image_id = state.scene.island.materials[i], \
                               .path = filepath,                            \
                               .buffer = SG_RANGE(state.file_buffer)})
  LOAD_ISLAND_IMAGE(0, "assets/island/OutsSS00.png");
  LOAD_ISLAND_IMAGE(1, "assets/island/OutsMM03.png");
  LOAD_ISLAND_IMAGE(2, "assets/island/OutsMM02.png");
  LOAD_ISLAND_IMAGE(3, "assets/island/OutsSS01.png");
  LOAD_ISLAND_IMAGE(4, "assets/island/OutsSS05.png");
  LOAD_ISLAND_IMAGE(5, "assets/island/OutsSS04.png");
  LOAD_ISLAND_IMAGE(6, "assets/island/OutsSS07.png");
  LOAD_ISLAND_IMAGE(7, "assets/island/OutsSS06.png");

#define LOAD_SEA_IMAGE(i, filepath)                                        \
  state.scene.water.materials[i] = sg_alloc_image();                       \
  batteries::load_img({.image_id = state.scene.water.materials[i], \
                               .path = filepath,                           \
                               .buffer = SG_RANGE(state.file_buffer)})
  LOAD_SEA_IMAGE(0, "assets/island/Sea.SS00.png");
  LOAD_SEA_IMAGE(1, "assets/island/Sea.SS01.png");
  LOAD_SEA_IMAGE(2, "assets/island/Sea.SS02.png");
  LOAD_SEA_IMAGE(3, "assets/island/Sea.SS04.png");
  LOAD_SEA_IMAGE(4, "assets/island/Sea.SS05.png");
  LOAD_SEA_IMAGE(5, "assets/island/Sea.SS06.png");
  LOAD_SEA_IMAGE(6, "assets/island/Sea.SS07.png");
  LOAD_SEA_IMAGE(7, "assets/island/Sea.SS08.png");

#undef LOAD_ISLAND_IMAGE
#undef LOAD_SEA_IMAGE
}

void create_windwaker_pass(void)
{
  state.windwaker.pass_action = (sg_pass_action){
      .colors[0] = {
          .clear_value = {state.scene.ambient.color.r, state.scene.ambient.color.g, state.scene.ambient.color.b, 1.0f},
          .load_action = SG_LOADACTION_CLEAR,
      },
  };
}

void create_world_pass(void)
{
  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = windwaker_object_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_world_params_t),
              .uniforms = {
                  [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                  [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
              },
          },
      },
      .fs = {
          .source = windwaker_object_fs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(fs_world_params_t),
              .uniforms = {
                  [0] = {.name = "ambient.intensity", .type = SG_UNIFORMTYPE_FLOAT},
                  [1] = {.name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                  [2] = {.name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
                  [3] = {.name = "palette.highlight", .type = SG_UNIFORMTYPE_FLOAT3},
                  [4] = {.name = "palette.shadow", .type = SG_UNIFORMTYPE_FLOAT3},
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
                  .glsl_name = "albedo",
                  .image_slot = 0,
                  .sampler_slot = 0,
              },
          },
      },
  };

  state.world_pass.pip = sg_make_pipeline({
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
      .depth = {
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .label = "object-pipeline",
  });

  state.world_pass.bind = (sg_bindings){
      .vertex_buffers[0] = state.scene.island.model.mesh.vbuf,
      .fs = {
          .samplers[0] = sg_make_sampler({
              .min_filter = SG_FILTER_LINEAR,
              .mag_filter = SG_FILTER_LINEAR,
              .wrap_u = SG_WRAP_REPEAT,
              .wrap_v = SG_WRAP_REPEAT,
              .label = "object-sampler",
          }),
      },
  };
}

void create_water_pass(void)
{
  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = windwaker_sea_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_world_params_t),
              .uniforms = {
                  [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                  [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
              },
          },
      },
      .fs = {
          .source = windwaker_sea_fs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(fs_water_params_t),
              .uniforms = {
                  [0] = {.name = "ambient.intensity", .type = SG_UNIFORMTYPE_FLOAT},
                  [1] = {.name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                  [2] = {.name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
                  [3] = {.name = "palette.highlight", .type = SG_UNIFORMTYPE_FLOAT3},
                  [4] = {.name = "palette.shadow", .type = SG_UNIFORMTYPE_FLOAT3},
                  [5] = {.name = "water.time", .type = SG_UNIFORMTYPE_FLOAT},
                  [6] = {.name = "water.color", .type = SG_UNIFORMTYPE_FLOAT3},
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
                  .glsl_name = "albedo",
                  .image_slot = 0,
                  .sampler_slot = 0,
              },
          },
      },
  };

  state.water_pass.pip = sg_make_pipeline({
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
      .depth = {
          // .pixel_format = SG_PIXELFORMAT_DEPTH,
          // .bias = 0.005f,
          .compare = SG_COMPAREFUNC_LESS,
          .write_enabled = true,
      },
      .label = "water-pipeline",
  });

  state.water_pass.bind = (sg_bindings){
      .vertex_buffers[0] = state.scene.water.model.mesh.vbuf,
      .fs = {
          .samplers[0] = sg_make_sampler({
              .min_filter = SG_FILTER_LINEAR,
              .mag_filter = SG_FILTER_LINEAR,
              .wrap_u = SG_WRAP_REPEAT,
              .wrap_v = SG_WRAP_REPEAT,
              .label = "water-sampler",
          }),
      },
  };
}

void init(void)
{
  boilerplate::setup();

  load_island();
  create_windwaker_pass();
  create_world_pass();
  create_water_pass();

  state.scene.island.model.transform.scale = glm::vec3(0.05f);
  state.scene.water.model.transform.scale = glm::vec3(0.05f);
}

void draw_ui(void)
{
  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::Text("%.1f, %.1f, %.1f", state.camera.position.x, state.camera.position.y, state.camera.position.z);

  ImGui::Text("Presets");
  if (ImGui::Button("Sunny Day"))
  {
    state.scene.palette = state.pal[0];
  }
  if (ImGui::Button("Bright Night"))
  {
    state.scene.palette = state.pal[1];
  }
  if (ImGui::Button("Rainy Day"))
  {
    state.scene.palette = state.pal[2];
  }
  if (ImGui::Button("Rainy Night"))
  {
    state.scene.palette = state.pal[3];
  }

  ImGui::Text("Palette");
  ImGui::ColorEdit3("Highlight", &state.scene.palette.highlight[0]);
  ImGui::ColorEdit3("Shadow", &state.scene.palette.shadow[0]);

  ImGui::End();
}

void frame(void)
{
  boilerplate::frame();
  draw_ui();

  const auto t = (float)sapp_frame_duration();
  state.camera_controller.update(&state.camera, t * 30);

  state.time.frame = (float)sapp_frame_duration();
  if (!state.time.paused)
  {
    state.time.absolute += state.time.frame * state.time.factor;
  }

  // math required by the scene
  const auto light_pos = glm::vec4(115.0f, 75.0f, 850.0f, 0.0f);
  state.scene.ambient.direction = glm::normalize(light_pos);

  fastObjGroup *group = nullptr;

  // graphics pass
  sg_begin_pass({.action = state.windwaker.pass_action, .swapchain = sglue_swapchain()});

  // initialize uniform data
  const vs_world_params_t vs_params = {
      .view_proj = state.camera.projection() * state.camera.view(),
      .model = state.scene.island.model.transform.matrix(),
  };
  const fs_world_params_t fs_params = {
      .ambient = state.scene.ambient,
      .palette = state.scene.palette,
  };

  // world pipeline
  sg_apply_pipeline(state.world_pass.pip);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
  sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));

  group = nullptr;
  for (auto i = 0; i < state.scene.island.model.mesh.obj->group_count; i++)
  {
    state.world_pass.bind.fs.images[0] = state.scene.island.materials[i];
    sg_apply_bindings(&state.world_pass.bind);

    group = state.scene.island.model.mesh.obj->groups + i;
    sg_draw(group->index_offset, group->face_count * 3, 1);
  }

  // initialize uniform data
  const vs_world_params_t vs_water_params = {
      .view_proj = state.camera.projection() * state.camera.view(),
      .model = state.scene.water.model.transform.matrix(),
  };
  const fs_water_params_t fs_water_params = {
      .ambient = state.scene.ambient,
      .palette = state.scene.palette,
      .water = {
          .time = (float)state.time.absolute,
          .color = glm::vec3(0.00f, 0.31f, 0.85f),
      }};

  // water pipeline
  sg_apply_pipeline(state.water_pass.pip);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_water_params));
  sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_water_params));

  group = nullptr;
  for (auto i = 0; i < state.scene.water.model.mesh.obj->group_count; i++)
  {
    state.water_pass.bind.fs.images[0] = state.scene.water.materials[i];
    sg_apply_bindings(&state.water_pass.bind);

    group = state.scene.water.model.mesh.obj->groups + i;
    sg_draw(group->index_offset, group->face_count * 3, 1);
  }

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