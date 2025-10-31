#include "scene.h"

// imgui
#include "imgui/imgui.h"

// ew
#include "ew/procGen.h"

// opengl
#if defined(SOKOL_GLCORE)
#include "gl3w/gl3w.h"
#else
#include <GLES3/gl3.h>
#endif

#include <iostream>

GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

static struct
{
    glm::vec3 color = {0.00f, 0.31f, 0.85f};
    glm::vec2 direction = {0.5f, 0.5f};
    float scale = 10.0f;
    float strength = 1.0f;
    float tiling = 10.0f;
    float top_scale = 0.90f;
    float bottom_scale = 0.02f;
    float lod_bias = 14.5f;
} debug;

Scene::Scene()
{
    water = std::make_unique<ew::Shader>("assets/shaders/windwaker/water.vs", "assets/shaders/windwaker/water.fs");
    texture = std::make_unique<ew::Texture>("assets/windwaker/water.png");

    water_mipmap = std::make_unique<ew::Texture>((ew::mipmap_t){
        "assets/windwaker/water128.png",
        "assets/windwaker/water64.png",
        "assets/windwaker/water32.png",
        "assets/windwaker/water16.png",
        "assets/windwaker/water8.png",
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    // set bindings
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, water_mipmap->getID());

    water->use();

    // samplers
    water->setInt("texture0", 0);

    // scene matrices
    water->setMat4("model", glm::mat4{1.0f});
    water->setMat4("view_proj", view_proj);

    // water properties
    water->setVec3("color", debug.color);
    water->setVec2("direction", debug.direction);
    water->setFloat("tiling", debug.tiling);
    water->setFloat("time", (float)time.absolute);
    water->setFloat("Ts", debug.top_scale);
    water->setFloat("Bs", debug.bottom_scale);
    water->setFloat("strength", debug.strength);
    water->setFloat("scale", debug.scale);
    water->setFloat("lod_bias", debug.lod_bias);

    plane.draw();

    glCheckError();
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::Separator();

    ImGui::SliderFloat("LOD Bias", &debug.lod_bias, 0.0f, 100.0f);

    if (ImGui::CollapsingHeader("Color"))
    {
        ImGui::ColorEdit3("Color", &debug.color[0]);
        ImGui::SliderFloat("Upper Blend", &debug.top_scale, 0.0f, 1.0f);
        ImGui::SliderFloat("Bottom Blend", &debug.bottom_scale, 0.0f, 1.0f);
    }
    if (ImGui::CollapsingHeader("Texture"))
    {
        ImGui::SliderFloat("Tilling", &debug.tiling, 1.0f, 100.0f);
        ImGui::SliderFloat2("Scroll Direction", &debug.direction[0], -1.0f, 1.0f);
    }
    if (ImGui::CollapsingHeader("Wave"))
    {
        ImGui::SliderFloat("Scale", &debug.scale, 1.0f, 100.0f);
        ImGui::SliderFloat("Strength", &debug.strength, 0.0f, 5.0f);
    }

    ImGui::End();
}