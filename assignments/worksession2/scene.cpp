#include "scene.h"

// batteries
#include "batteries/math.h"
#include "batteries/opengl.h"

// imgui
#include "imgui/imgui.h"

// ew
#include "ew/procGen.h"

Scene::Scene()
{
    island = std::make_unique<ew::Shader>("assets/shaders/island.vs", "assets/shaders/island.fs");
    water_shader = std::make_unique<ew::Shader>("assets/shaders/island_water.vs", "assets/shaders/island_water.fs");
    heightmap = std::make_unique<ew::Texture>("assets/heightmaps/heightmap.png");

    plane.load(ew::createPlane(50.0f, 50.0f, 100));
    water_plane.load(ew::createPlane(50.0f, 50.0f, 1));
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set bindings
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heightmap->getID());

    island->use();

    // samplers
    island->setInt("heightmap", 0);

    // scene matrices
    island->setMat4("model", glm::mat4{1.0f});
    island->setMat4("view_proj", view_proj);

    // island properties
    island->setFloat("landmass.scale", 10.0f);

    plane.draw();

    water_shader->use();
    water_shader->setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.7f, 0.0f)));
    water_shader->setMat4("view_proj", view_proj);
    water_shader->setVec3("camera_position", camera.position);
    water_plane.draw();
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::End();
}