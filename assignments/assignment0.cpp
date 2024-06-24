#define BATTERIES_IMPL
#include "batteries.h"

//
// Assignment0 -- Blinn Phong
//

// shaders
#include "shaders/blinn_phong.h"

typedef struct
{
  glm::mat4 view_proj;
  glm::mat4 model;
  glm::vec3 eye;
} vs_blinnphong_params_t;

typedef struct
{
  batteries::material_t material;
  batteries::ambient_t ambient;
} fs_blinnphong_params_t;

// application state
static struct
{
  uint8_t file_buffer[batteries::megabytes(5)];

  struct
  {
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
  } blinnphong;

  batteries::camera_t camera;
  batteries::camera_controller_t camera_controller;
  batteries::ambient_t ambient;

  struct
  {
    float ry;
    batteries::model_t suzanne;
    batteries::material_t material;
  } scene;
} state = {
    .ambient = {
        .intensity = 1.0f,
        .color = glm::vec3(0.25f, 0.45f, 0.65f),
    },
    .scene = {
        .ry = 0.0f,
        .material = {
            .Ka = 1.0f,
            .Kd = 0.5f,
            .Ks = 0.5f,
            .Shininess = 128.0f,
        },
    },
};

void load_suzanne(void)
{
  state.scene.suzanne.mesh.vbuf = sg_alloc_buffer();
  batteries::assets::load_obj({
      .buffer_id = state.scene.suzanne.mesh.vbuf,
      .mesh = &state.scene.suzanne.mesh,
      .path = "assets/suzanne.obj",
      .buffer = SG_RANGE(state.file_buffer),
  });
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
                  [2] = {.name = "eye", .type = SG_UNIFORMTYPE_FLOAT3},
              },
          },
      },
      .fs = {
          .source = blinn_phong_fs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(fs_blinnphong_params_t),
              .uniforms = {
                  [0] = {.name = "material.Ka", .type = SG_UNIFORMTYPE_FLOAT},
                  [1] = {.name = "material.Kd", .type = SG_UNIFORMTYPE_FLOAT},
                  [2] = {.name = "material.Ks", .type = SG_UNIFORMTYPE_FLOAT},
                  [3] = {.name = "material.Shininess", .type = SG_UNIFORMTYPE_FLOAT},
                  [4] = {.name = "ambient.intensity", .type = SG_UNIFORMTYPE_FLOAT},
                  [5] = {.name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                  [6] = {.name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},

              },
          },
      },
  };

  state.blinnphong.pass_action = (sg_pass_action){
      .colors[0] = {
          .clear_value = {
              state.ambient.color.r * state.ambient.intensity,
              state.ambient.color.g * state.ambient.intensity,
              state.ambient.color.b * state.ambient.intensity,
              1.0f,
          },
          .load_action = SG_LOADACTION_CLEAR,
      },
  };

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
      .depth = {
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
  batteries::setup();

  load_suzanne();
  create_blinnphong_pass();
}

static void update_clear_color(void)
{
  state.blinnphong.pass_action.colors[0].clear_value = {
      state.ambient.color.r * state.ambient.intensity,
      state.ambient.color.g * state.ambient.intensity,
      state.ambient.color.b * state.ambient.intensity,
      1.0f,
  };
}

void draw_ui(void)
{
  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  if (ImGui::CollapsingHeader("Ambient Light"))
  {
    if (ImGui::SliderFloat("Intensity", &state.ambient.intensity, 0.0f, 1.0f))
    {
      update_clear_color();
    }
    if (ImGui::ColorEdit3("Color", &state.ambient.color[0]))
    {
      update_clear_color();
    }
  }
  if (ImGui::CollapsingHeader("Material"))
  {
    ImGui::SliderFloat("Ambient", &state.scene.material.Ka, 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse", &state.scene.material.Kd, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular", &state.scene.material.Ks, 0.0f, 1.0f);
    ImGui::SliderFloat("Shininess", &state.scene.material.Shininess, 2.0f, 1024.0f);
  }
  ImGui::End();
}

void frame(void)
{
  batteries::frame();
  draw_ui();

  const auto t = (float)sapp_frame_duration();
  state.camera_controller.update(&state.camera, t);
  state.scene.ry += 0.2f * t;

  // sugar: rotate suzzane
  state.scene.suzanne.transform.rotation = glm::rotate(state.scene.suzanne.transform.rotation, t, glm::vec3(0.0, 1.0, 0.0));

  // sugar: rotate light
  const auto rym = glm::rotate(state.scene.ry, glm::vec3(0.0f, 1.0f, 0.0f));
  const auto light_pos = rym * glm::vec4(50.0f, 50.0f, -50.0f, 1.0f);
  state.ambient.direction = glm::normalize(light_pos);

  // graphics pass
  sg_begin_pass({.action = state.blinnphong.pass_action, .swapchain = sglue_swapchain()});
  sg_apply_pipeline(state.blinnphong.pip);
  sg_apply_bindings(&state.blinnphong.bind);

  // initialize uniform data
  const vs_blinnphong_params_t vs_params = {
      .view_proj = state.camera.projection() * state.camera.view(),
      .model = state.scene.suzanne.transform.matrix(),
      .eye = state.camera.position,
  };
  const fs_blinnphong_params_t fs_params = {
      .material = state.scene.material,
      .ambient = state.ambient,
  };

  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
  sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
  sg_draw(0, state.scene.suzanne.mesh.num_faces * 3, 1);

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