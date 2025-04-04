#include "scene.h"

// imgui
#include "imgui/imgui.h"

// batteries
#include "batteries/materials.h"
#include "batteries/math.h"

// opengl
#include <GLES3/gl3.h>

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};

enum
{
    PBR_ALBEDO = 0,
    PBR_METALLIC = 1,
    PBR_ROUGHNESS = 2,
    PBR_OCCLUSION = 3,
    PBR_SPECULAR = 4,
};

Scene::Scene()
{
    shell = std::make_unique<ew::Model>("assets/smashbros/greenshell/greenshell.obj");
    pbr = std::make_unique<ew::Shader>("assets/shaders/pbr.vs", "assets/shaders/pbr.fs");
    textures.push_back(std::make_unique<ew::Texture>("assets/smashbros/greenshell/greenshell_col.png"));
    textures.push_back(std::make_unique<ew::Texture>("assets/smashbros/greenshell/greenshell_mtl.png"));
    textures.push_back(std::make_unique<ew::Texture>("assets/smashbros/greenshell/greenshell_rgh.png"));
    textures.push_back(std::make_unique<ew::Texture>("assets/smashbros/greenshell/greenshell_ao.png"));
    textures.push_back(std::make_unique<ew::Texture>("assets/smashbros/greenshell/greenshell_spc.png"));

    ambient = {
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    light = {
        .brightness = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
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
    glBindTexture(GL_TEXTURE_2D, textures[PBR_ALBEDO]->getID());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[PBR_METALLIC]->getID());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[PBR_ROUGHNESS]->getID());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textures[PBR_OCCLUSION]->getID());

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, textures[PBR_SPECULAR]->getID());

    pbr->use();

    // scene matrices
    pbr->setMat4("model", glm::mat4{1.0f});
    pbr->setMat4("view_proj", view_proj);
    pbr->setVec3("camera_position", camera.position);

    // pbr material properties
    pbr->setInt("material.albedo", PBR_ALBEDO);
    pbr->setInt("material.metallic", PBR_METALLIC);
    pbr->setInt("material.roughness", PBR_ROUGHNESS);
    pbr->setInt("material.occlusion", PBR_OCCLUSION);
    pbr->setInt("material.specular", PBR_SPECULAR);

    // point light
    pbr->setVec3("light.color", light.color);
    pbr->setVec3("light.position", light.position);

    // draw suzanne
    shell->draw();
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::End();
}