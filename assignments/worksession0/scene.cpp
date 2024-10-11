#include "scene.h"

#include "imgui/imgui.h"

#include "batteries/assets.h"

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};

Scene::Scene()
{
    ambient = (batteries::ambient_t){
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    light = (batteries::light_t){
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
    };

    // load_togezoshell();
    togezoshell.Load("assets/smashbros/togezoshell/togezoshell.obj");
    togezoshell.transform.rotation = glm::rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    togezoshell.transform.scale = glm::vec3(0.5f);
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
    const PhysicallyBasedRendering::vs_params_t vs_pbr_params = {
        .view_proj = view_proj,
        .model = togezoshell.transform.matrix(),
    };
    const PhysicallyBasedRendering::fs_params_t fs_pbr_params = {
        .light = light,
        .camera_position = camera.position,
    };
    const batteries::Gizmo::vs_params_t vs_gizmo_params = {
        .view_proj = view_proj,
        .model = glm::translate(glm::mat4(1.0f), light.position),
    };
    const batteries::Gizmo::fs_params_t fs_gizmo_params = {
        .color = light.color,
    };
    const batteries::Skybox::vs_params_t vs_skybox_params = {
        .view_proj = camera.projection() * glm::mat4(glm::mat3(camera.view())),
    };

    framebuffer.RenderTo([&]()
                         {
        pbr.Apply(vs_pbr_params, fs_pbr_params);
        togezoshell.Render();
        gizmo.Render(vs_gizmo_params, fs_gizmo_params); });

    framebuffer.Render();
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);
    ImGui::End();
}