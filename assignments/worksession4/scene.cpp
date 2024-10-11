#include "scene.h"
#include "imgui/imgui.h"

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};

static ToonShading::Palette pal[]{
    {.highlight = {1.00f, 1.00f, 1.00f}, .shadow = {0.60f, 0.54f, 0.52f}},
    {.highlight = {0.47f, 0.58f, 0.68f}, .shadow = {0.32f, 0.39f, 0.57f}},
    {.highlight = {0.62f, 0.69f, 0.67f}, .shadow = {0.50f, 0.55f, 0.50f}},
    {.highlight = {0.24f, 0.36f, 0.54f}, .shadow = {0.25f, 0.31f, 0.31f}},
};

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

    skull.Load("assets/windwaker/skull/skull.obj");
    // skull.Load("assets/windwaker/pot_water/pot_water.obj");
    skull.transform.scale = glm::vec3(0.05f);
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
    const ToonShading::vs_params_t vs_toon_params = {
        .view_proj = view_proj,
        .model = skull.transform.matrix(),
    };
    const ToonShading::fs_params_t fs_toon_params = {
        .light = light,
        .palette = palette,
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
        toonshading.Apply(vs_toon_params, fs_toon_params);
        skull.Render(); });
    // gizmo.Render(vs_gizmo_params, fs_gizmo_params);
    // skybox.Render(vs_skybox_params); });

    framebuffer.Render();
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Presets");
    if (ImGui::Button("Sunny Day"))
    {
        palette = pal[0];
    }
    if (ImGui::Button("Bright Night"))
    {
        palette = pal[1];
    }
    if (ImGui::Button("Rainy Day"))
    {
        palette = pal[2];
    }
    if (ImGui::Button("Rainy Night"))
    {
        palette = pal[3];
    }

    ImGui::Text("Palette");
    ImGui::ColorEdit3("Highlight", &palette.highlight[0]);
    ImGui::ColorEdit3("Shadow", &palette.shadow[0]);

    ImGui::End();
}