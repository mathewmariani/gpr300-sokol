#define BATTERIES_IMPL
#include "batteries.h"

#include "fast_obj/fast_obj.h"

#include <array>
#include <string>
#include <vector>

// shaders
#include "shaders/blinn_phong.h"
#include "shaders/blur_post_process.h"
#include "shaders/grayscale_post_process.h"
#include "shaders/inverse_post_process.h"

typedef struct
{
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;
  glm::vec3 eye;
  glm::vec3 ambient;
} vs_params_t;

std::vector<std::string> post_processing_effects = {
    "Grayscale",
    "Kernel Blur",
    "Inverse",
};

// application state
static struct
{
  struct
  {
    sg_pass_action pass_action;
    sg_pass pass;
    sg_pipeline pip;
    sg_bindings bind;
  } offscreen;

  struct
  {
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
  } display;

  int effect_index;

  uint8_t file_buffer[3 * 1024 * 1024];

  batteries::model_t suzanne;
  glm::vec3 ambient_light;

  struct
  {
    bool suzanne;
    bool failed;
  } loaded;
} state = {
    .effect_index = 0,
    .ambient_light = glm::vec3(0.25f, 0.45f, 0.65f),
    .loaded = {
        .suzanne = false,
        .failed = false,
    },
};

void create_offscreen_pass()
{
  const auto width = sapp_width();
  const auto height = sapp_height();

  // create a render pass object with one color and one depth render attachment image
  auto img_desc = (sg_image_desc){
      .render_target = true,
      .width = width,
      .height = height,
  };

  // color attachment
  img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
  img_desc.label = "color-image";
  auto color_img = sg_make_image(&img_desc);

  // depth attachment
  img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
  img_desc.label = "depth-image";
  auto depth_img = sg_make_image(&img_desc);

  // create an image sampler
  auto color_smplr = sg_make_sampler((sg_sampler_desc){
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
  });

  state.offscreen.pass = sg_make_pass((sg_pass_desc){
      .color_attachments[0].image = color_img,
      .depth_stencil_attachment.image = depth_img,
      .label = "offscreen-pass",
  });

  auto offscreen_shader_desc = (sg_shader_desc){
      .vs = {
          .source = blinn_phong_vs,
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
          .source = blinn_phong_fs,
      },
  };

  state.offscreen.pass_action = (sg_pass_action){
      .colors[0] = {
          .clear_value = {state.ambient_light.r, state.ambient_light.g, state.ambient_light.b, 1.0f},
          .load_action = SG_LOADACTION_CLEAR,
      }};

  state.offscreen.pip = sg_make_pipeline((sg_pipeline_desc){
      .layout = {
          .attrs = {
              [0].format = SG_VERTEXFORMAT_FLOAT3,
              [1].format = SG_VERTEXFORMAT_FLOAT3,
          },
      },
      .shader = sg_make_shader(offscreen_shader_desc),
      .index_type = SG_INDEXTYPE_NONE,
      .cull_mode = SG_CULLMODE_FRONT,
      .depth = {
          .pixel_format = SG_PIXELFORMAT_DEPTH,
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .colors[0].pixel_format = SG_PIXELFORMAT_RGBA8,
      .label = "offscreen-pipeline",
  });

  // bind image and sampler
  state.display.bind.fs.images[0] = color_img;
  state.display.bind.fs.samplers[0] = color_smplr;
}

void create_display_pass()
{
  static std::vector<std::array<std::string, 2>> shader_stage = {
      {grayscale_post_process_vs, grayscale_post_process_fs},
      {blur_post_process_vs, blur_post_process_fs},
      {inverse_post_process_vs, inverse_post_process_fs},
  };

  float quad_vertices[] = {
      -1.0f, 1.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 1.0f, 0.0f,

      -1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 1.0f, 1.0f};

  auto quad_buffer = sg_make_buffer((sg_buffer_desc){
      .data = {
          .ptr = &quad_vertices,
          .size = sizeof(quad_vertices),
      },
      .label = "quad-vertices",
  });

  auto display_shader_desc = (sg_shader_desc){
      .vs = {
          .source = shader_stage[state.effect_index][0].c_str(),
      },
      .fs = {
          .source = shader_stage[state.effect_index][1].c_str(),
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

  state.display.pip = sg_make_pipeline((sg_pipeline_desc){
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
  state.display.bind.vertex_buffers[0] = quad_buffer;
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

      /* setup resource bindings */
      state.offscreen.bind.vertex_buffers[0] = state.suzanne.mesh.vbuf;

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

void init(void)
{
  batteries::setup();
  create_offscreen_pass();
  create_display_pass();

  // start loading data
  sfetch_send((sfetch_request_t){
      .path = "assets/suzanne.obj",
      .callback = suzanne_data_loaded,
      .buffer = SFETCH_RANGE(state.file_buffer),
  });
}

void frame(void)
{
  batteries::frame();

  const auto width = sapp_width();
  const auto height = sapp_height();

  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  if (ImGui::ColorEdit3("Ambient Light", &state.ambient_light[0]))
  {
    state.offscreen.pass_action.colors[0].clear_value = {state.ambient_light.r, state.ambient_light.g, state.ambient_light.b, 1.0f};
  }
  if (ImGui::BeginCombo("Effect", post_processing_effects[0].c_str()))
  {
    for (auto n = 0; n < post_processing_effects.size(); ++n)
    {
      auto is_selected = (post_processing_effects[state.effect_index] == post_processing_effects[n]);
      if (ImGui::Selectable(post_processing_effects[n].c_str(), is_selected))
      {
        state.effect_index = n;
        create_display_pass();
      }
      if (is_selected)
      {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }
  ImGui::End();

  if (!state.loaded.failed && (state.loaded.suzanne))
  {
    state.suzanne.transform.rotation = glm::rotate(state.suzanne.transform.rotation, (float)sapp_frame_duration(), glm::vec3(0.0, 1.0, 0.0));

    const vs_params_t vs_params = {
        .proj = glm::perspective(glm::radians(60.0f), (float)(width / (float)height), 0.01f, 10.0f),
        .view = glm::lookAt(glm::vec3(0.0f, 1.5f, 6.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
        .model = state.suzanne.transform.matrix(),
        .eye = glm::vec3(0.0f, 1.5f, 6.0f),
        .ambient = state.ambient_light,
    };

    sg_begin_pass(state.offscreen.pass, &state.offscreen.pass_action);
    sg_apply_pipeline(state.offscreen.pip);
    sg_apply_bindings(&state.offscreen.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));

    sg_draw(0, state.suzanne.mesh.num_faces * 3, 1);

    sg_end_pass();
  }

  // draws the fullscreen quad
  sg_begin_default_pass(&state.display.pass_action, width, height);
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