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
  glm::vec3 ambient;
  glm::vec3 light_dir;
} vs_display_params_t;

typedef struct
{
  float Ka, Kd, Ks;
  float Shininess;
} fs_display_params_t;

// application state
static struct
{
  struct
  {
    sg_pass_action pass_action;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;
  } display;

  struct
  {
    float ry;
    glm::vec3 ambient_light;
    batteries::model_t suzanne;
    batteries::material_t material;
  } scene;

  uint8_t file_buffer[batteries::megabytes(5)];
} state = {
    .scene = {
        .ry = 0.0f,
        .ambient_light = glm::vec3(0.25f, 0.45f, 0.65f),
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
  batteries::assets::load_obj((batteries::assets::obj_request_t){
      .buffer_id = state.scene.suzanne.mesh.vbuf,
      .mesh = &state.scene.suzanne.mesh,
      .path = "assets/suzanne.obj",
      .buffer = SG_RANGE(state.file_buffer),
  });
}

void create_display_pass(void)
{
  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = blinn_phong_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_display_params_t),
              .uniforms = {
                  [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                  [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                  [2] = {.name = "eye", .type = SG_UNIFORMTYPE_FLOAT3},
                  [3] = {.name = "ambient", .type = SG_UNIFORMTYPE_FLOAT3},
                  [4] = {.name = "light_dir", .type = SG_UNIFORMTYPE_FLOAT3},
              },
          },
      },
      .fs = {
          .source = blinn_phong_fs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(fs_display_params_t),
              .uniforms = {
                  [0] = {.name = "material.Ka", .type = SG_UNIFORMTYPE_FLOAT},
                  [1] = {.name = "material.Kd", .type = SG_UNIFORMTYPE_FLOAT},
                  [2] = {.name = "material.Ks", .type = SG_UNIFORMTYPE_FLOAT},
                  [3] = {.name = "material.Shininess", .type = SG_UNIFORMTYPE_FLOAT},
              },
          },
      },
  };

  state.display.pass_action = (sg_pass_action){
      .colors[0] = {
          .clear_value = {state.scene.ambient_light.r, state.scene.ambient_light.g, state.scene.ambient_light.b, 1.0f},
          .load_action = SG_LOADACTION_CLEAR,
      }};

  state.display.pip = sg_make_pipeline((sg_pipeline_desc){
      .layout = {
          .attrs = {
              [0].format = SG_VERTEXFORMAT_FLOAT3,
              [1].format = SG_VERTEXFORMAT_FLOAT3,
              [2].format = SG_VERTEXFORMAT_FLOAT2,
          }},
      .shader = sg_make_shader(shader_desc),
      .index_type = SG_INDEXTYPE_NONE,
      .face_winding = SG_FACEWINDING_CCW,
      .cull_mode = SG_CULLMODE_BACK,
      .depth = {
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .label = "display-pipeline",
  });

  state.display.bind = (sg_bindings){
      .vertex_buffers[0] = state.scene.suzanne.mesh.vbuf,
  };
}

void init(void)
{
  batteries::setup();

  load_suzanne();
  create_display_pass();
}

void frame(void)
{
  batteries::frame();

  const auto t = (float)sapp_frame_duration();
  state.scene.ry += 0.2f * t;

  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  if (ImGui::ColorEdit3("Ambient Light", &state.scene.ambient_light[0]))
  {
    state.display.pass_action.colors[0].clear_value = {state.scene.ambient_light.r, state.scene.ambient_light.g, state.scene.ambient_light.b, 1.0f};
  }
  if (ImGui::CollapsingHeader("Material"))
  {
    ImGui::SliderFloat("Ambient", &state.scene.material.Ka, 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse", &state.scene.material.Kd, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular", &state.scene.material.Ks, 0.0f, 1.0f);
    ImGui::SliderFloat("Shininess", &state.scene.material.Shininess, 2.0f, 1024.0f);
  }
  ImGui::End();

  const auto width = sapp_width();
  const auto height = sapp_height();

  // math required by the scene
  auto camera_pos = glm::vec3(0.0f, 1.5f, 6.0f);
  auto camera_proj = glm::perspective(glm::radians(60.0f), (float)(width / (float)height), 0.01f, 10.0f);
  auto camera_view = glm::lookAt(camera_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
  auto camera_view_proj = camera_proj * camera_view;

  // sugar: rotate suzzane
  state.scene.suzanne.transform.rotation = glm::rotate(state.scene.suzanne.transform.rotation, t, glm::vec3(0.0, 1.0, 0.0));

  // sugar: rotate light
  const glm::mat4 rym = glm::rotate(state.scene.ry, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::vec3 light_pos = rym * glm::vec4(50.0f, 50.0f, -50.0f, 1.0f);

  // initialize uniform data
  const vs_display_params_t vs_params = {
      .view_proj = camera_view_proj,
      .model = state.scene.suzanne.transform.matrix(),
      .eye = camera_pos,
      .ambient = state.scene.ambient_light,
      .light_dir = glm::normalize(light_pos),
  };
  const fs_display_params_t fs_params = {
      .Ka = state.scene.material.Ka,
      .Kd = state.scene.material.Kd,
      .Ks = state.scene.material.Ks,
      .Shininess = state.scene.material.Shininess,
  };

  // graphics pass
  sg_begin_default_pass(&state.display.pass_action, width, height);
  sg_apply_pipeline(state.display.pip);
  sg_apply_bindings(&state.display.bind);
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