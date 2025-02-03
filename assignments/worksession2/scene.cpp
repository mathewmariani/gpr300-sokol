// batteries
#include "scene.h"

// imgui
#include "imgui/imgui.h"

// ew
#include "ew/procGen.h"

// opengl
#include <GLES3/gl3.h>

Scene::Scene()
{
    #include "island_generator.glsl.h"

    island = std::make_unique<ew::Shader>(island_generator_vs, island_generator_fs);
    heightmap = std::make_unique<ew::Texture>("assets/heightmaps/heightmap.png");

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
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

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
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::End();
}