#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Work Session 5 -- Transitions
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"
#include "batteries/materials.h"
#include "batteries/lights.h"

// shaders
#include "shaders/transition.h"

typedef struct
{
  float cutoff;
  glm::vec3 color;
} transition_t;

typedef struct
{
  sg_image img[3];
  sg_sampler smp;
} gradient_t;

typedef struct
{
  transition_t transition;
} fs_display_params_t;

// application state
static struct
{
  uint8_t file_buffer[boilerplate::megabytes(5)];

  glm::vec3 backgroud;
  transition_t transition;
  gradient_t gradient;

  sg_pass_action pass_action;

  struct
  {
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
  } display;
} state = {
    .backgroud = glm::vec3(0.16f, 0.50f, 0.72f),
    .transition = {
        .cutoff = 0.0f,
        .color = glm::vec3(0.94f, 0.76f, 0.05f),
    },

};

void load_gradients(void)
{
  // create an image sampler
  state.gradient.smp = sg_make_sampler({
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
  });
  state.gradient.img[0] = sg_alloc_image();
  batteries::load_img({
      .image_id = state.gradient.img[0],
      .path = "assets/transitions/gradient1.png",
      .buffer = SG_RANGE(state.file_buffer),
  });
  state.gradient.img[1] = sg_alloc_image();
  batteries::load_img({
      .image_id = state.gradient.img[1],
      .path = "assets/transitions/gradient2.png",
      .buffer = SG_RANGE(state.file_buffer),
  });
  state.gradient.img[2] = sg_alloc_image();
  batteries::load_img({
      .image_id = state.gradient.img[2],
      .path = "assets/transitions/gradient3.png",
      .buffer = SG_RANGE(state.file_buffer),
  });
}

void create_display_pass(void)
{
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
          .source = transition_vs,
      },
      .fs = {
          .source = transition_fs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(fs_display_params_t),
              .uniforms = {
                  [0] = {.name = "transition.cutoff", .type = SG_UNIFORMTYPE_FLOAT},
                  [1] = {.name = "transition.color", .type = SG_UNIFORMTYPE_FLOAT3},
              },
          },
          .images[0].used = true,
          .samplers[0].used = true,
          .image_sampler_pairs[0] = {
              .glsl_name = "transition.gradient",
              .image_slot = 0,
              .sampler_slot = 0,
              .used = true,
          },
      },
  };

  state.display.pass_action = (sg_pass_action){
      .colors[0].load_action = SG_LOADACTION_DONTCARE,
      .depth.load_action = SG_LOADACTION_DONTCARE,
      .stencil.load_action = SG_LOADACTION_DONTCARE,
  };

  state.display.pip = sg_make_pipeline({
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
  state.display.bind = (sg_bindings){
      .vertex_buffers[0] = quad_buffer,
      .fs = {
          .images[0] = state.gradient.img[0],
          .samplers[0] = state.gradient.smp,
      },
  };
}

void init(void)
{
  boilerplate::setup();
  load_gradients();
  create_display_pass();
}

void draw_ui(void)
{
  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  ImGui::Text("Transition");
  ImGui::DragFloat("Cutoff", &state.transition.cutoff, 0.01f, 0.00f, 1.00f);

  ImGui::ColorEdit3("Background", &state.backgroud[0]);
  ImGui::ColorEdit3("Fadeout Color", &state.transition.color[0]);

  if (ImGui::Button("Gradient 1"))
  {
    state.display.bind.fs.images[0] = state.gradient.img[0];
  }
  if (ImGui::Button("Gradient 2"))
  {
    state.display.bind.fs.images[0] = state.gradient.img[1];
  }
  if (ImGui::Button("Gradient 3"))
  {
    state.display.bind.fs.images[0] = state.gradient.img[2];
  }

  ImGui::End();
}

void frame(void)
{
  boilerplate::frame();
  draw_ui();

  const auto t = (float)sapp_frame_duration();

  // initialize uniform data
  const fs_display_params_t fs_params = {
      .transition = state.transition,
  };

  // base layer
  sg_begin_pass({
      .action = (sg_pass_action){
          .colors[0] = {
              .load_action = SG_LOADACTION_CLEAR,
              .clear_value = {state.backgroud.r, state.backgroud.g, state.backgroud.b, 1.0f},
          },
      },
      .swapchain = sglue_swapchain(),
  });
  sg_end_pass();

  // transition layer
  sg_begin_pass({.action = state.display.pass_action, .swapchain = sglue_swapchain()});
  sg_apply_pipeline(state.display.pip);
  sg_apply_bindings(&state.display.bind);
  sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
  sg_draw(0, 6, 1);

  // draw ui
  simgui_render();

  sg_end_pass();
  sg_commit();
}

void event(const sapp_event *event)
{
  boilerplate::event(event);
}

void cleanup(void)
{
  boilerplate::shutdown();
}