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
    PBR_COMBINED = 5,
};

struct {
    bool use_custom = true;
    bool is_metal = false;
    float base_reflectivity = 0.04f;
} pbr_pipeline;

struct {
    glm::vec3 alb = {0.6f, 0.0f, 0.6f};
    float mtl = 1.0f;
    float rgh = 1.0f;
} pbr_material;

Scene::Scene()
{
    shell = std::make_unique<ew::Model>("assets/smashbros/greenshell/greenshell.obj");
    pbr = std::make_unique<ew::Shader>("assets/shaders/pbr.vs", "assets/shaders/pbr.fs");
    textures.push_back(std::make_unique<ew::Texture>("assets/smashbros/greenshell/greenshell_col.png"));
    textures.push_back(std::make_unique<ew::Texture>("assets/smashbros/greenshell/greenshell_mtl.png"));
    textures.push_back(std::make_unique<ew::Texture>("assets/smashbros/greenshell/greenshell_rgh.png"));
    textures.push_back(std::make_unique<ew::Texture>("assets/smashbros/greenshell/greenshell_ao.png"));
    textures.push_back(std::make_unique<ew::Texture>("assets/smashbros/greenshell/greenshell_spc.png"));
    textures.push_back(std::make_unique<ew::Texture>("assets/smashbros/greenshell/greenshell_pbr.png"));

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

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, textures[PBR_COMBINED]->getID());

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
    pbr->setInt("material.combined", PBR_COMBINED);

    // pbr pipeline
    pbr->setInt("is_metal", pbr_pipeline.is_metal);
    pbr->setInt("base_reflectivity", pbr_pipeline.base_reflectivity);
    pbr->setInt("use_custom", pbr_pipeline.use_custom);

    // pbr properties values
    pbr->setVec3("custom_alb", pbr_material.alb);
    pbr->setFloat("custom_mtl", pbr_material.mtl);
    pbr->setFloat("custom_rgh", pbr_material.rgh);

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

    ImGui::SeparatorText("PBR Pipeline");
    {
        ImGui::Checkbox("Is Metal", &pbr_pipeline.is_metal);
        ImGui::SliderFloat("Base Reflectivity", &pbr_pipeline.base_reflectivity, 0.0f, 1.0f);
        ImGui::Checkbox("Use Custom Material", &pbr_pipeline.use_custom); 
    }
    if (pbr_pipeline.use_custom)
    {
        ImGui::SeparatorText("Custom Material");
        {
            ImGui::ColorEdit3("Albedo", &pbr_material.alb[0]);
            ImGui::SliderFloat("Metallic", &pbr_material.mtl, 0.0f, 1.0f);
            ImGui::SliderFloat("Roughness", &pbr_material.rgh, 0.0f, 1.0f);
        }
    }

    ImGui::End();
}