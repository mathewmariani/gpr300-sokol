#include "scene.h"

#include "batteries/assets.h"

#include "imgui/imgui.h"

#include <array>
#include <string>
#include <vector>

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};
static uint8_t file_buffer[1024 * 1024 * 5];

static int effect_index = 0;
static std::vector<std::string> post_processing_effects = {
    "None",
    "Grayscale",
    "Kernel Blur",
    "Inverse",
    "Chromatic Aberration",
    "CRT",
};

Scene::Scene()
{
    auto load_suzanne = [this]()
    {
        suzanne.mesh.vbuf = sg_alloc_buffer();
        suzanne.mesh.bindings = (sg_bindings){
            .vertex_buffers[0] = suzanne.mesh.vbuf,
        };
        batteries::load_obj({
            .buffer_id = suzanne.mesh.vbuf,
            .mesh = &suzanne.mesh,
            .path = "assets/suzanne.obj",
            .buffer = SG_RANGE(file_buffer),
        });
    };

    ambient = (batteries::ambient_t){
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    light = (batteries::light_t){
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
    };

    load_suzanne();

    grayscaleRenderer.bindings.fs = framebuffer.bind.fs;
    blurRenderer.bindings.fs = framebuffer.bind.fs;
    inverseRenderer.bindings.fs = framebuffer.bind.fs;
    chromaticAberrationRenderer.bindings.fs = framebuffer.bind.fs;
    chromaticAberrationRenderer.bindings.fs = framebuffer.bind.fs;
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    static auto ry = 0.0f;
    ry += dt;

    // sugar: rotate light
    const auto rym = glm::rotate(ry, glm::vec3(0.0f, 1.0f, 0.0f));
    light.position = rym * light_orbit_radius;
}

void Scene::Render(void)
{
    const auto view_proj = camera.projection() * camera.view();

    // initialize uniform data
    const BlinnPhong::vs_params_t vs_blinnphong_params = {
        .view_proj = view_proj,
        .model = suzanne.transform.matrix(),
    };
    const BlinnPhong::fs_params_t fs_blinnphong_params = {
        .material = material,
        .light = light,
        .ambient = ambient,
        .camera_position = camera.position,
    };
    const batteries::Gizmo::vs_params_t vs_gizmo_params = {
        .view_proj = view_proj,
        .model = glm::translate(glm::mat4(1.0f), light.position),
    };
    const batteries::Gizmo::fs_params_t fs_gizmo_params = {
        .color = light.color,
    };

    // render using blinn-phong pipeline
    blinnPhong.Apply(vs_blinnphong_params, fs_blinnphong_params);
    suzanne.Render();

    gizmo.Render(vs_gizmo_params, fs_gizmo_params);

    // apply a post processing effect
    switch (effect_index)
    {
    case 1:
        framebuffer.ApplyEffect(&grayscaleRenderer);
        break;
    case 2:
        framebuffer.ApplyEffect(&blurRenderer);
        break;
    case 3:
        framebuffer.ApplyEffect(&inverseRenderer);
        break;
    case 4:
        framebuffer.ApplyEffect(&chromaticAberrationRenderer);
        break;
    default:
        framebuffer.ApplyEffect(nullptr);
        break;
    }
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);
    if (ImGui::CollapsingHeader("Ambient"))
    {
        ImGui::SliderFloat("Intensity", &ambient.intensity, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color", &ambient.color[0]);
    }
    if (ImGui::CollapsingHeader("Light"))
    {
        ImGui::SliderFloat("Brightness", &light.brightness, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color", &light.color[0]);
    }
    if (ImGui::BeginCombo("Effect", post_processing_effects[effect_index].c_str()))
    {
        for (auto n = 0; n < post_processing_effects.size(); ++n)
        {
            auto is_selected = (post_processing_effects[effect_index] == post_processing_effects[n]);
            if (ImGui::Selectable(post_processing_effects[n].c_str(), is_selected))
            {
                effect_index = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::End();
}