#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Assignment1 -- Post Processing
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"
#include "batteries/materials.h"
#include "batteries/lights.h"

// shaders
#include "shaders/blinn_phong.h"
#include "shaders/no_post_process.h"
#include "shaders/chromatic_aberration.h"
#include "shaders/crt_post_process.h"
#include "shaders/blur_post_process.h"
#include "shaders/grayscale_post_process.h"
#include "shaders/inverse_post_process.h"

#include <array>
#include <string>
#include <vector>

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

std::vector<std::string> post_processing_effects = {
    "None",
    "Grayscale",
    "Kernel Blur",
    "Inverse",
    "Chromatic Aberration",
    "CRT",
};

// application state
static struct
{
  uint8_t file_buffer[boilerplate::megabytes(4)];

  struct
  {
    sg_image color;
    sg_image depth;
    sg_sampler sampler;
  } framebuffer;

  struct
  {
    sg_pass_action pass_action;
    sg_attachments attachments;
    sg_pipeline pip;
    sg_bindings bind;
  } blinnphong;

  struct
  {
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
  } postprocess;

  int effect_index;

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
    .effect_index = 0,
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
  batteries::load_obj({
      .buffer_id = state.scene.suzanne.mesh.vbuf,
      .mesh = &state.scene.suzanne.mesh,
      .path = "assets/suzanne.obj",
      .buffer = SG_RANGE(state.file_buffer),
  });
}

void create_framebuffer(void)
{
  const auto width = sapp_width();
  const auto height = sapp_height();

  // color attachment
  state.framebuffer.color = sg_make_image((sg_image_desc){
      .pixel_format = SG_PIXELFORMAT_RGBA8,
      .render_target = true,
      .width = width,
      .height = height,
      .label = "color-image",
  });

  // depth attachment
  state.framebuffer.depth = sg_make_image((sg_image_desc){
      .pixel_format = SG_PIXELFORMAT_DEPTH,
      .render_target = true,
      .width = width,
      .height = height,
      .label = "depth-image",
  });

  // create an image sampler
  state.framebuffer.sampler = sg_make_sampler({
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
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
          .clear_value = {state.ambient.color.r, state.ambient.color.g, state.ambient.color.b, 1.0f},
          .load_action = SG_LOADACTION_CLEAR,
      },
  };

  state.blinnphong.attachments = sg_make_attachments((sg_attachments_desc){
      .colors[0].image = state.framebuffer.color,
      .depth_stencil.image = state.framebuffer.depth,
  });

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
          .pixel_format = SG_PIXELFORMAT_DEPTH,
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .label = "display-pipeline",
  });

  state.blinnphong.bind = (sg_bindings){
      .vertex_buffers[0] = state.scene.suzanne.mesh.vbuf,
  };
}

void create_postprocess_pass()
{
  static std::vector<std::array<std::string, 2>> shader_stage = {
      {no_post_process_vs, no_post_process_fs},
      {grayscale_post_process_vs, grayscale_post_process_fs},
      {blur_post_process_vs, blur_post_process_fs},
      {inverse_post_process_vs, inverse_post_process_fs},
      {chromatic_aberration_vs, chromatic_aberration_fs},
      {crt_post_process_vs, crt_post_process_fs},
  };

  // clang-format off
  float quad_vertices[] = {
     -1.0f, 1.0f, 0.0f, 1.0f,
     -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 1.0f, 0.0f,

     -1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 1.0f, 1.0f
  };
  // clang-format on

  auto quad_buffer = sg_make_buffer({
      .data = SG_RANGE(quad_vertices),
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

  state.postprocess.pass_action = (sg_pass_action){
      .colors[0].load_action = SG_LOADACTION_CLEAR,
      .depth.load_action = SG_LOADACTION_DONTCARE,
      .stencil.load_action = SG_LOADACTION_DONTCARE,
  };

  state.postprocess.pip = sg_make_pipeline({
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
  state.postprocess.bind = (sg_bindings){
      .vertex_buffers[0] = quad_buffer,
      .fs = {
          .images[0] = state.framebuffer.color,
          .samplers[0] = state.framebuffer.sampler,
      },
  };
}

void init(void)
{
  boilerplate::setup();
  load_suzanne();
  create_framebuffer();
  create_blinnphong_pass();
  create_postprocess_pass();
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
  if (ImGui::BeginCombo("Effect", post_processing_effects[state.effect_index].c_str()))
  {
    for (auto n = 0; n < post_processing_effects.size(); ++n)
    {
      auto is_selected = (post_processing_effects[state.effect_index] == post_processing_effects[n]);
      if (ImGui::Selectable(post_processing_effects[n].c_str(), is_selected))
      {
        state.effect_index = n;
        create_postprocess_pass();
      }
      if (is_selected)
      {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }
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
  boilerplate::frame();
  draw_ui();

  const auto t = (float)sapp_frame_duration();
  state.camera_controller.update(&state.camera, t);
  state.scene.ry += 0.2f * t;

  // math required by the scene
  // sugar: rotate suzzane
  state.scene.suzanne.transform.rotation = glm::rotate(state.scene.suzanne.transform.rotation, t, glm::vec3(0.0, 1.0, 0.0));

  // sugar: rotate light
  const auto rym = glm::rotate(state.scene.ry, glm::vec3(0.0f, 1.0f, 0.0f));
  const auto light_pos = rym * glm::vec4(50.0f, 50.0f, -50.0f, 1.0f);
  state.ambient.direction = glm::normalize(light_pos);

  // offscreen pass
  sg_begin_pass({.action = state.blinnphong.pass_action, .attachments = state.blinnphong.attachments});
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
  sg_end_pass();

  // draws the fullscreen quad
  sg_begin_pass({.action = state.postprocess.pass_action, .swapchain = sglue_swapchain()});
  sg_apply_pipeline(state.postprocess.pip);
  sg_apply_bindings(&state.postprocess.bind);
  sg_draw(0, 6, 1);

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