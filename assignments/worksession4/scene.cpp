#include "scene.h"
#include "imgui/imgui.h"

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};

static int palette_index = 0;
static std::vector<std::tuple<std::string, ToonShading::Palette>> palette{
    {"Sunny Day", {.highlight = {1.00f, 1.00f, 1.00f}, .shadow = {0.60f, 0.54f, 0.52f}}},
    {"Bright Night", {.highlight = {0.47f, 0.58f, 0.68f}, .shadow = {0.32f, 0.39f, 0.57f}}},
    {"Rainy Day", {.highlight = {0.62f, 0.69f, 0.67f}, .shadow = {0.50f, 0.55f, 0.50f}}},
    {"Rainy Night", {.highlight = {0.24f, 0.36f, 0.54f}, .shadow = {0.25f, 0.31f, 0.31f}}},
};

static int model_index = 0;
static std::vector<std::string> model_paths{
    "assets/windwaker/skull/skull.obj",
    "assets/windwaker/pot_water/pot_water.obj",
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

    models.resize(model_paths.size());
    auto size = model_paths.size();
    for (auto i = 0; i < size; ++i)
    {
        models[i].Load(model_paths[i]);
        models[i].transform.scale = glm::vec3(0.05f);
    }
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
    auto model = models[model_index];

    // initialize uniform data
    const ToonShading::vs_params_t vs_toon_params = {
        .view_proj = view_proj,
        .model = model.transform.matrix(),
    };
    const ToonShading::fs_params_t fs_toon_params = {
        .light = light,
        .palette = std::get<ToonShading::Palette>(palette[palette_index]),
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
        model.Render(); });
    // gizmo.Render(vs_gizmo_params, fs_gizmo_params);
    // skybox.Render(vs_skybox_params); });

    framebuffer.Render();
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Presets");
    if (ImGui::BeginCombo("Palette", std::get<std::string>(palette[palette_index]).c_str()))
    {
        for (auto n = 0; n < palette.size(); ++n)
        {
            auto is_selected = (std::get<0>(palette[palette_index]) == std::get<0>(palette[n]));
            if (ImGui::Selectable(std::get<std::string>(palette[n]).c_str(), is_selected))
            {
                palette_index = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::ColorEdit3("Highlight", &std::get<ToonShading::Palette>(palette[palette_index]).highlight[0]);
    ImGui::ColorEdit3("Shadow", &std::get<ToonShading::Palette>(palette[palette_index]).shadow[0]);

    ImGui::Text("Model");
    if (ImGui::BeginCombo("Model", model_paths[model_index].c_str()))
    {
        for (auto n = 0; n < model_paths.size(); ++n)
        {
            auto is_selected = (model_paths[model_index] == model_paths[n]);
            if (ImGui::Selectable(model_paths[n].c_str(), is_selected))
            {
                model_index = n;
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