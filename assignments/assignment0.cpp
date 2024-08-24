#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Assignment0 -- Blinn Phong
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"

#include "batteries/framebuffer.h"
#include "batteries/blinnphong.h"
#include "batteries/gizmo.h"

// stl
#include <unordered_map>

static constexpr glm::vec4 light_orbit_radius = glm::vec4(2.0f, 0.0f, 2.0f, 1.0f);

// http://devernay.free.fr/cours/opengl/materials.html
static std::unordered_map<std::string, batteries::material_t> material_map = {
    {"emerald", {{0.0215f, 0.1745f, 0.0215f}, {0.07568f, 0.61424f, 0.07568f}, {0.633f, 0.727811f, 0.633f}, 0.6f}},
    {"jade", {{0.135f, 0.2225f, 0.1575f}, {0.54f, 0.89f, 0.63f}, {0.316228f, 0.316228f, 0.316228f}, 0.1}},
    {"obsidian", {{0.05375f, 0.05f, 0.06625f}, {0.18275f, 0.17f, 0.22525f}, {0.332741f, 0.328634f, 0.346435f}, 0.3f}},
    {"pearl", {{0.25f, 0.20725f, 0.20725f}, {1.0f, 0.829f, 0.829f}, {0.296648f, 0.296648f, 0.296648f}, 0.088f}},
    {"ruby", {{0.1745f, 0.01175f, 0.01175f}, {0.61424f, 0.04136f, 0.04136f}, {0.727811f, 0.626959f, 0.626959f}, 0.6f}},
    {"turquoise", {{0.1f, 0.18725f, 0.1745f}, {0.396f, 0.74151f, 0.69102f}, {0.297254f, 0.30829f, 0.306678f}, 0.1f}},
    {"brass", {{0.329412f, 0.223529f, 0.027451f}, {0.780392f, 0.568627f, 0.113725f}, {0.992157f, 0.941176f, 0.807843f}, 0.21794872f}},
    {"bronze", {{0.2125f, 0.1275f, 0.054f}, {0.714f, 0.4284f, 0.18144f}, {0.393548f, 0.271906f, 0.166721f}, 0.2f}},
    {"chrome", {{0.25f, 0.25f, 0.25f}, {0.4f, 0.4f, 0.4f}, {0.774597f, 0.774597f, 0.774597f}, 0.6f}},
    {"copper", {{0.19125f, 0.0735f, 0.0225f}, {0.7038f, 0.27048f, 0.0828f}, {0.256777f, 0.137622f, 0.086014f}, 0.1f}},
    {"gold", {{0.24725f, 0.1995f, 0.0745f}, {0.75164f, 0.60648f, 0.22648f}, {0.628281f, 0.555802f, 0.366065f}, 0.4f}},
    {"silver", {{0.19225f, 0.19225f, 0.19225f}, {0.50754f, 0.50754f, 0.50754f}, {0.508273f, 0.508273f, 0.508273f}, 0.4f}},
    {"black plastic", {{0.0f, 0.0f, 0.0f}, {0.01f, 0.01f, 0.01f}, {0.50f, 0.50f, 0.50f}, 0.25f}},
    {"cyan plastic", {{0.0f, 0.1f, 0.06f}, {0.0f, 0.50980392f, 0.50980392f}, {0.50196078f, 0.50196078f, 0.50196078f}, 0.25f}},
    {"green plastic", {{0.0f, 0.0f, 0.0f}, {0.1f, 0.35f, 0.1f}, {0.45f, 0.55f, 0.45f}, 0.25f}},
    {"red plastic", {{0.0f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, {0.7f, 0.6f, 0.6f}, 0.25f}},
    {"white plastic", {{0.0f, 0.0f, 0.0f}, {0.55f, 0.55f, 0.55f}, {0.70f, 0.70f, 0.70f}, 0.25f}},
    {"yellow plastic", {{0.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.0f}, {0.60f, 0.60f, 0.50f}, 0.25f}},
    {"black rubber", {{0.02f, 0.02f, 0.02f}, {0.01f, 0.01f, 0.01f}, {0.4f, 0.4f, 0.4f}, 0.078125f}},
    {"cyan rubber", {{0.0f, 0.05f, 0.05f}, {0.4f, 0.5f, 0.5f}, {0.04f, 0.7f, 0.7f}, 0.078125f}},
    {"green rubber", {{0.0f, 0.05f, 0.0f}, {0.4f, 0.5f, 0.4f}, {0.04f, 0.7f, 0.04f}, 0.078125f}},
    {"red rubber", {{0.05f, 0.0f, 0.0f}, {0.5f, 0.4f, 0.4f}, {0.7f, 0.04f, 0.04f}, 0.078125f}},
    {"white rubber", {{0.05f, 0.05f, 0.05f}, {0.5f, 0.5f, 0.5f}, {0.7f, 0.7f, 0.7f}, 0.078125f}},
    {"yellow rubber", {{0.05f, 0.05f, 0.0f}, {0.5f, 0.5f, 0.4f}, {0.7f, 0.7f, 0.04f}, 0.078125f}},
};

// application state
static struct
{
  uint8_t file_buffer[boilerplate::megabytes(5)];

  batteries::framebuffer_t framebuffer;
  batteries::blinnphong_t blinnphong;
  batteries::gizmo_t gizmo;

  batteries::camera_t camera;
  batteries::camera_controller_t camera_controller;
  batteries::light_t light;

  struct
  {
    float ry;
    batteries::model_t suzanne;
    batteries::material_t material;
  } scene;
} state = {
    .light = {
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
    },
    .scene = {
        .ry = 0.0f,
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

void init(void)
{
  const auto width = sapp_width();
  const auto height = sapp_height();

  boilerplate::setup();
  batteries::create_framebuffer(&state.framebuffer, width, height);
  batteries::create_blinnphong_pass(&state.blinnphong);
  batteries::create_gizmo_pass(&state.gizmo);

  load_suzanne();

  // apply bindings
  state.blinnphong.bind = (sg_bindings){
      .vertex_buffers[0] = state.scene.suzanne.mesh.vbuf,
  };

  state.scene.material = material_map.at("emerald");
}

void draw_ui(void)
{
  ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  if (ImGui::CollapsingHeader("Light"))
  {
    ImGui::SliderFloat("Brightness", &state.light.brightness, 0.0f, 1.0f);
    ImGui::ColorEdit3("Color", &state.light.color[0]);
  }

  static int current_item = 0; // Index for currently selected item
  int item_index = 0;          // Used to track the index of the current item in the loop
  std::string selected_material_name;

  if (ImGui::BeginCombo("Material", material_map.begin()->first.c_str()))
  {
    for (const auto &pair : material_map)
    {
      bool is_selected = (current_item == item_index);
      if (ImGui::Selectable(pair.first.c_str(), is_selected))
      {
        current_item = item_index;
        state.scene.material = pair.second;
      }
      if (is_selected)
      {
        ImGui::SetItemDefaultFocus();
      }
      item_index++;
    }
    ImGui::EndCombo();
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

  // sugar: rotate light
  const auto rym = glm::rotate(state.scene.ry, glm::vec3(0.0f, 1.0f, 0.0f));
  state.light.position = rym * light_orbit_radius;

  const auto view_proj = state.camera.projection() * state.camera.view();

  // initialize uniform data
  const batteries::vs_blinnphong_params_t vs_params = {
      .view_proj = view_proj,
      .model = state.scene.suzanne.transform.matrix(),
  };
  const batteries::fs_blinnphong_params_t fs_params = {
      .material = state.scene.material,
      .light = state.light,
      .camera_position = state.camera.position,
  };
  const batteries::vs_gizmo_params_t vs_gizmo_params = {
      .view_proj = view_proj,
      .model = glm::translate(glm::mat4(1.0f), state.light.position),
  };
  const batteries::fs_gizmo_light_params_t fs_gizmo_light_params = {
      .light_color = state.light.color,
  };

  // blinn-phong pass
  sg_begin_pass({.action = state.blinnphong.action, .attachments = state.framebuffer.attachments});
  sg_apply_pipeline(state.blinnphong.pip);
  sg_apply_bindings(&state.blinnphong.bind);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
  sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
  sg_draw(0, state.scene.suzanne.mesh.num_faces * 3, 1);
  sg_end_pass();

  // render light sources
  sg_begin_pass({.action = state.gizmo.action, .attachments = state.framebuffer.attachments});
  sg_apply_pipeline(state.gizmo.pip);
  sg_apply_bindings(&state.gizmo.bind);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_gizmo_params));
  sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_gizmo_light_params));
  sg_draw(state.gizmo.sphere.base_element, state.gizmo.sphere.num_elements, 1);
  sg_end_pass();

  // display pass
  sg_begin_pass({.action = state.framebuffer.action, .swapchain = sglue_swapchain()});
  sg_apply_pipeline(state.framebuffer.pip);
  sg_apply_bindings(&state.framebuffer.bind);
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