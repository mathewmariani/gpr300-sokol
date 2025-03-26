#include "scene.h"

// batteries
#include "batteries/materials.h"
#include "batteries/math.h"

// imgui
#include "imgui/imgui.h"

// opengl
#include <GLES3/gl3.h>

#include <tuple>
#include <vector>

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 2.0f};

struct Palette
{
    glm::vec3 highlight;
    glm::vec3 shadow;
};

static int palette_index = 0;
static std::vector<std::tuple<std::string, Palette>> palette{
    {"Sunny Day", {.highlight = {1.00f, 1.00f, 1.00f}, .shadow = {0.60f, 0.54f, 0.52f}}},
    {"Bright Night", {.highlight = {0.47f, 0.58f, 0.68f}, .shadow = {0.32f, 0.39f, 0.57f}}},
    {"Rainy Day", {.highlight = {0.62f, 0.69f, 0.67f}, .shadow = {0.50f, 0.55f, 0.50f}}},
    {"Rainy Night", {.highlight = {0.24f, 0.36f, 0.54f}, .shadow = {0.25f, 0.31f, 0.31f}}},
};

Scene::Scene()
{
    toonshading = std::make_unique<ew::Shader>("assets/shaders/windwaker/toonshading.vs", "assets/shaders/windwaker/toonshading.fs");
    skull = std::make_unique<ew::Model>("assets/windwaker/skull/skull.obj");
    texture = std::make_unique<ew::Texture>("assets/windwaker/skull/Txo_dokuo.png");
    zatoon = std::make_unique<ew::Texture>("assets/windwaker/ZAtoon.png");

    ambient = {
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    light = {
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
    };
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    const auto rym = glm::rotate((float)time.absolute, glm::vec3(0.0f, 1.0f, 0.0f));
    light.position = rym * light_orbit_radius;
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    // set bindings
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->getID());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, zatoon->getID());

    toonshading->use();

    // scene matrices
    toonshading->setMat4("model", glm::scale(glm::mat4(1.0f), glm::vec3(0.05f)));
    toonshading->setMat4("view_proj", view_proj);
    toonshading->setVec3("camera_position", camera.position);

    // samplers
    toonshading->setInt("material.albedo", 0);
    toonshading->setInt("material.zatoon", 1);

    // ambient light
    toonshading->setVec3("palette.highlight", std::get<1>(palette[palette_index]).highlight);
    toonshading->setVec3("palette.shadow", std::get<1>(palette[palette_index]).shadow);

    // point light
    toonshading->setVec3("light.color", light.color);
    toonshading->setVec3("light.position", light.position);

    // draw skull
    skull->draw();
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::Separator();

    ImGui::Text("Presets");
    if (ImGui::BeginCombo("Palette", std::get<std::string>(palette[palette_index]).c_str()))
    {
        for (auto n = 0; n < palette.size(); ++n)
        {
            auto is_selected = (std::get<0>(palette[palette_index]) == std::get<0>(palette[n]));
            if (ImGui::Selectable(std::get<0>(palette[n]).c_str(), is_selected))
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
    ImGui::ColorEdit3("Highlight", &std::get<1>(palette[palette_index]).highlight[0]);
    ImGui::ColorEdit3("Shadow", &std::get<1>(palette[palette_index]).shadow[0]);

    ImGui::End();
}