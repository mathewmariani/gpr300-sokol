// batteries
#include "scene.h"

// imgui
#include "imgui/imgui.h"

// ew
#include "ew/procGen.h"

// opengl
#include <GLES3/gl3.h>

static struct
{
    float lod_bias = 14.5f;
    float tiling = 100.0f;
    float time;
    float top_scale = 1.00f;
    float bottom_scale = 1.00f;
    float brightness_lower_cutoff = 0.40f;
    float brightness_upper_cutoff = 0.93f;
} debug;

Scene::Scene()
{
    water = std::make_unique<ew::Shader>("assets/shaders/sunshine/water.vs", "assets/shaders/sunshine/water.fs");
    texture = std::make_unique<ew::Texture>((ew::mipmap_t){
        "assets/sunshine/water128.png",
        "assets/sunshine/water64.png",
        "assets/sunshine/water32.png",
        "assets/sunshine/water16.png",
        "assets/sunshine/water8.png",
    });

    plane.load(ew::createPlane(400.0f, 400.0f, 10));
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
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    // set bindings
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->getID());

    water->use();

    // samplers
    water->setInt("texture0", 0);

    // scene matrices
    water->setMat4("model", glm::mat4{1.0f});
    water->setMat4("view_proj", view_proj);

    // water properties
    water->setFloat("lod_bias", debug.lod_bias);
    water->setFloat("tiling", debug.tiling);
    water->setFloat("time", (float)time.absolute);
    water->setFloat("top_scale", debug.top_scale);
    water->setFloat("bottom_scale", debug.bottom_scale);
    water->setFloat("brightness_lower_cutoff", debug.brightness_lower_cutoff);
    water->setFloat("brightness_upper_cutoff", debug.brightness_upper_cutoff);

    plane.draw();
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::Separator();

    ImGui::SliderFloat("LOD Bias", &debug.lod_bias, 0.0f, 100.0f);
    ImGui::SliderFloat("brightness_lower_cutoff", &debug.brightness_lower_cutoff, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("brightness_upper_cutoff", &debug.brightness_upper_cutoff, 0.0f, 1.0f, "%.2f");

    if (ImGui::CollapsingHeader("Texture"))
    {
        ImGui::SliderFloat("debug.tiling", &debug.tiling, 1.0f, 1000.0f);
        ImGui::SliderFloat("debug.top_scale", &debug.top_scale, 0.0f, 1.0f);
        ImGui::SliderFloat("debug.bottom_scale", &debug.bottom_scale, 0.0f, 1.0f);
    }

    ImGui::End();
}