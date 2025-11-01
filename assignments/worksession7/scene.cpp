#include "scene.h"

// batteries
#include "batteries/opengl.h"

// imgui
#include "imgui/imgui.h"

// ew
#include "ew/procGen.h"

struct
{
    glm::vec3 color = {0.2627f, 0.5255f, 0.6431f};
    // glm::vec3 color = {0.00f, 0.31f, 0.85f};
    float scale = 5.0f;
    float warp_scale = 1.0f;
    float spec_scale = 1.0f;
    float warp_strength = 0.2f;
    float brightness_lower_cutoff = 0.2f;
    float brightness_upper_cutoff = 0.9f;
} debug;

Scene::Scene()
{
    shader = std::make_unique<ew::Shader>("assets/doubledash/water.vs", "assets/doubledash/water.fs");
    water_spec = std::make_unique<ew::Texture>("assets/doubledash/wave_spec.png");
    water_tex = std::make_unique<ew::Texture>("assets/doubledash/wave_tex.png");
    water_warp = std::make_unique<ew::Texture>("assets/doubledash/wave_warp.png");

    plane.load(ew::createPlane(50.0f, 50.0f, 100));
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
    glBindTexture(GL_TEXTURE_2D, water_spec->getID());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, water_tex->getID());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, water_warp->getID());

    shader->use();

    // samplers
    shader->setInt("water_spec", 0);
    shader->setInt("water_tex", 1);
    shader->setInt("water_warp", 2);

    // scene matrices
    shader->setMat4("model", glm::mat4{1.0f});
    shader->setMat4("view_proj", view_proj);

    // debug tools
    shader->setVec3("color", debug.color);
    shader->setVec3("camera_pos", camera.position);
    shader->setFloat("time", (float)time.absolute);
    shader->setFloat("scale", debug.scale);
    shader->setFloat("spec_scale", debug.spec_scale);
    shader->setFloat("warp_scale", debug.warp_scale);
    shader->setFloat("warp_strength", debug.warp_strength);
    shader->setFloat("brightness_lower_cutoff", debug.brightness_lower_cutoff);
    shader->setFloat("brightness_upper_cutoff", debug.brightness_upper_cutoff);

    plane.draw();
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::SliderFloat("Scale", &debug.scale, 1.0f, 10.0f);
    ImGui::SliderFloat("spec_scale", &debug.spec_scale, 1.0f, 10.0f);
    ImGui::SliderFloat("warp_scale", &debug.warp_scale, 1.0f, 10.0f);
    ImGui::SliderFloat("warp_strength", &debug.warp_strength, 0.0f, 1.0f);
    ImGui::SliderFloat("brightness_lower_cutoff", &debug.brightness_lower_cutoff, 0.0f, 1.0f);
    ImGui::SliderFloat("brightness_upper_cutoff", &debug.brightness_upper_cutoff, 0.0f, 1.0f);

    ImGui::End();
}