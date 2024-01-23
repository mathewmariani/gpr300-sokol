// sokol
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_fetch.h"
#include "sokol/sokol_time.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_glue.h"

// imgui
#include "imgui/imgui.h"

// sokol-imgui
#define SOKOL_IMGUI_IMPL
#include "sokol/sokol_imgui.h"

// libs
#include "glm/glm.hpp"
#include "fast_obj/fast_obj.h"

#define BATTERIES_IMPL
#include "batteries.h"

#include <string>

typedef struct
{
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
  glm::vec3 eye;
  glm::vec3 ambient;
} vs_params_t;

// FIXME: move this to assets
const std::string vs_source = R"(#version 300 es
// attributes
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 WorldPos; // Vertex position in world space
out vec3 WorldNormal; // Vertex normal in world space

void main()
{
  WorldPos = vec3(model * vec4(vPos, 1.0));
  WorldNormal = transpose(inverse(mat3(model))) * vNormal;
  gl_Position = projection * view * model * vec4(vPos, 1.0);
})";

// FIXME: move this to assets
const std::string fs_source = R"(#version 300 es
precision mediump float;

out vec4 FragColor;

in vec3 WorldPos;
in vec3 WorldNormal;

uniform vec3 eye;
uniform vec3 ambient;

vec3 _LightDirection = vec3(0.0, -1.0, 0.0);
vec3 _LightColor = vec3(1.0);

void main()
{
  // Make sure fragment normal is still length 1 after interpolation.
  vec3 normal = normalize(WorldNormal);

  // Light pointing straight down
  vec3 toLight = -_LightDirection;
  float diffuseFactor = max(dot(normal, toLight), 0.0);

	// Direction towards eye
	vec3 toEye = normalize(eye - WorldPos);

	// Blinn-phong uses half angle
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal, h), 0.0), 128.0);

	// Combination of specular and diffuse reflection
	vec3 lightColor = (diffuseFactor + specularFactor) * _LightColor;
	vec3 objectColor = vec3(normal * 0.5 + 0.5);

  // Add some ambient light
  lightColor += ambient;

	FragColor = vec4(objectColor * lightColor, 1.0);
})";

static struct
{
  sg_pass_action pass_action;

  batteries::model_t suzanne;

  // lights
  glm::vec3 ambient_light;

  struct
  {
    bool suzanne;
    bool failed;
  } loaded;
} state = {
    .ambient_light = glm::vec3(0.25f, 0.5f, 0.25f),
    .loaded = {
        .suzanne = false,
        .failed = false,
    },
};

static uint8_t mesh_io_buffer[3 * 1024 * 1024];
static void suzanne_data_loaded(const sfetch_response_t *response);

void init(void)
{
  // setup sokol-gfx
  sg_setup((sg_desc){
      .context = sapp_sgcontext(),
      .logger.func = slog_func,
  });

  // setup sokol-time
  stm_setup();

  // setup sokol-fetch
  sfetch_setup((sfetch_desc_t){
      .max_requests = 3,
      .num_channels = 1,
      .num_lanes = 3,
      .logger.func = slog_func,
  });

  // setup sokol-imgui
  simgui_setup((simgui_desc_t){
      .logger.func = slog_func,
  });

  // initialize pass action for default-pass
  state.pass_action = (sg_pass_action){
      .colors[0] = {
          .load_action = SG_LOADACTION_CLEAR,
          .clear_value = {state.ambient_light.r, state.ambient_light.g, state.ambient_light.b, 1.0f},
      },
  };

  sg_shader shader = sg_make_shader((sg_shader_desc){
      .vs = {
          .source = vs_source.c_str(),
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_params_t),
              .uniforms = {
                  [0] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                  [1] = {.name = "view", .type = SG_UNIFORMTYPE_MAT4},
                  [2] = {.name = "projection", .type = SG_UNIFORMTYPE_MAT4},
                  [3] = {.name = "eye", .type = SG_UNIFORMTYPE_FLOAT3},
                  [4] = {.name = "ambient", .type = SG_UNIFORMTYPE_FLOAT3},
              },
          },
      },
      .fs = {
          .source = fs_source.c_str(),
      },
  });

  state.suzanne.mesh.pip = sg_make_pipeline((sg_pipeline_desc){
      .layout = {
          .attrs = {
              [0].format = SG_VERTEXFORMAT_FLOAT3,
              [1].format = SG_VERTEXFORMAT_FLOAT3,
          }},
      .shader = shader,
      // .index_type = SG_INDEXTYPE_UINT16,
      .index_type = SG_INDEXTYPE_NONE,
      .cull_mode = SG_CULLMODE_FRONT,
      .depth = {
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .label = "display-pipeline",
  });

  // start loading data
  sfetch_send((sfetch_request_t){
      .path = "assets/suzanne.obj",
      .callback = suzanne_data_loaded,
      .buffer = SFETCH_RANGE(mesh_io_buffer),
  });
}

static void suzanne_data_loaded(const sfetch_response_t *response)
{
  if (response->fetched)
  {
    auto *mesh = fast_obj_read((const char *)response->data.ptr, response->data.size);
    if (mesh)
    {
      // https://stackoverflow.com/a/36454139
      // a face has 3 vertices, so multiply by 3.
      state.suzanne.mesh.num_faces = mesh->face_count;
      for (auto i = 0; i < mesh->face_count * 3; ++i)
      {
        auto vertex = mesh->indices[i];
        // vertex
        state.suzanne.mesh.vertices.push_back(*((mesh->positions + vertex.p * 3) + 0));
        state.suzanne.mesh.vertices.push_back(*((mesh->positions + vertex.p * 3) + 1));
        state.suzanne.mesh.vertices.push_back(*((mesh->positions + vertex.p * 3) + 2));
        // normal
        state.suzanne.mesh.vertices.push_back(*((mesh->normals + vertex.n * 3) + 0));
        state.suzanne.mesh.vertices.push_back(*((mesh->normals + vertex.n * 3) + 1));
        state.suzanne.mesh.vertices.push_back(*((mesh->normals + vertex.n * 3) + 2));
      }

      fast_obj_destroy(mesh);

      state.suzanne.mesh.vbuf = sg_make_buffer((sg_buffer_desc){
          .data = {
              .ptr = state.suzanne.mesh.vertices.data(),
              .size = state.suzanne.mesh.vertices.size() * sizeof(float),
          },
          .label = "suzanne-vertices",
      });

      // TODO: include index buffer
      // state.suzanne.mesh.ibuf = sg_make_buffer((sg_buffer_desc){
      //     .type = SG_BUFFERTYPE_INDEXBUFFER,
      //     .data = {
      //         .ptr = state.suzanne.mesh.indices.data(),
      //         .size = state.suzanne.mesh.indices.size() * sizeof(float),
      //     },
      //     .label = "suzanne-indices",
      // });

      /* setup resource bindings */
      state.suzanne.mesh.bind = (sg_bindings){
          .vertex_buffers[0] = state.suzanne.mesh.vbuf,
          // .index_buffer = state.suzanne.mesh.ibuf,
      };

      state.loaded.suzanne = true;
    }
    else
    {
      state.loaded.failed = true;
    }
  }
  else if (response->failed)
  {
    state.loaded.failed = true;
  }
}

void frame(void)
{
  sfetch_dowork();

  const float width = sapp_widthf();
  const float height = sapp_heightf();

  simgui_new_frame({(int)width, (int)height, sapp_frame_duration(), sapp_dpi_scale()});

  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  if (ImGui::ColorEdit3("Ambient Light", &state.ambient_light[0]))
  {
    state.pass_action.colors[0].clear_value = {state.ambient_light.r, state.ambient_light.g, state.ambient_light.b, 1.0f};
  }
  ImGui::End();

  sg_begin_default_pass(&state.pass_action, width, height);

  if (!state.loaded.failed && (state.loaded.suzanne))
  {
    state.suzanne.transform.rotation = glm::rotate(state.suzanne.transform.rotation, (float)sapp_frame_duration(), glm::vec3(0.0, 1.0, 0.0));

    const vs_params_t vs_params = {
        .proj = glm::perspective(glm::radians(60.0f), width / height, 0.01f, 10.0f),
        .view = glm::lookAt(glm::vec3(0.0f, 1.5f, 6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        .model = state.suzanne.transform.matrix(),
        .eye = glm::vec3(0.0f, 1.5f, 6.0f),
        .ambient = state.ambient_light,
    };

    sg_apply_pipeline(state.suzanne.mesh.pip);
    sg_apply_bindings(&state.suzanne.mesh.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));

    sg_draw(0, state.suzanne.mesh.num_faces * 3, 1);
  }

  simgui_render();
  sg_end_pass();
  sg_commit();
}

void event(const sapp_event *event)
{
  simgui_handle_event(event);
}

void cleanup(void)
{
  sfetch_shutdown();
  simgui_shutdown();
  sg_shutdown();
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