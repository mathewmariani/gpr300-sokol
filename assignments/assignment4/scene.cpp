#include "scene.h"
#include "catnumrollspline.h"

// imgui
#include "imgui/imgui.h"

// opengl
#include <GLES3/gl3.h> 

Scene::Scene()
{
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
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::End();
}