#include "scene.h"

// batteries
#include "batteries/math.h"

// imgui
#include "imgui/imgui.h"

// ew
#include "ew/procGen.h"

// opengl
#include <GLES3/gl3.h>

constexpr int kFramebufferWidth = 800;
constexpr int kFramebufferHeight = 600;

static glm::vec4 light_orbit_radius = {10.0f, 15.0f, 10.0f, 1.0f};

struct Framebuffer
{
    GLuint fbo;
    GLuint color0;
    GLuint depth;

    void Initialize()
    {
        // initialize framebuffer
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // position attachment
        glGenTextures(1, &color0);
        glBindTexture(GL_TEXTURE_2D, color0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, kFramebufferWidth, kFramebufferHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color0, 0);

        // Create depth texture
        glGenTextures(1, &depth);
        glBindTexture(GL_TEXTURE_2D, depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, kFramebufferWidth, kFramebufferHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

        // check completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Not so victorious\n");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

enum
{
	WATER_REFLECTION = 0,
	WATER_REFRACTION = 1,
	WATER_COUNT,
};

Framebuffer waterBuffers[WATER_COUNT];

struct
{
	float land_scale = 10.0f;
	float water_height = 2.0f;
	float wave_speed = 2.0f;
	float refreaction_power = 5.0f;

    float distortion_strength = 0.02f;
    float distortion_scale = 5.0f;
    float refract_strength = 10.0f;
} debug;

Scene::Scene()
{
    terrain_shader = std::make_unique<ew::Shader>("assets/project0/terrain.vs", "assets/project0/terrain.fs");
    water_shader = std::make_unique<ew::Shader>("assets/project0/water.vs", "assets/project0/water.fs");
    light_shader = std::make_unique<ew::Shader>("assets/project0/light.vs", "assets/project0/light.fs");

    dudv = std::make_unique<ew::Texture>("assets/project0/dudv.png");
    normal = std::make_unique<ew::Texture>("assets/project0/normal.png");
    heightmap = std::make_unique<ew::Texture>("assets/heightmaps/heightmap.png");

    terrain_plane.load(ew::createPlane(50.0f, 50.0f, 100));
    water_plane.load(ew::createPlane(50.0f, 50.0f, 1));
    light_sphere.load(ew::createSphere(1.0f, 8));

    waterBuffers[WATER_REFLECTION].Initialize();
	waterBuffers[WATER_REFRACTION].Initialize();

    light.color = glm::vec3(1.0f);
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

void Scene::RenderTerrain(const glm::vec4 clip_plane)
{
	const auto view_proj = camera.Projection() * camera.View();

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightmap->getID());

	terrain_shader->use();
	terrain_shader->setInt("heightmap", 0);
	terrain_shader->setMat4("model", glm::mat4(1.0f));
	terrain_shader->setMat4("view_proj", view_proj);
	terrain_shader->setFloat("landmass.scale", debug.land_scale);
	terrain_shader->setVec4("clip_plane", clip_plane);

	terrain_plane.draw();
}

void Scene::RenderWater(void)
{
	const auto view_proj = camera.Projection() * camera.View();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterBuffers[WATER_REFLECTION].color0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterBuffers[WATER_REFRACTION].color0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, dudv->getID());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normal->getID());
    glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, waterBuffers[WATER_REFRACTION].depth);

	water_shader->use();
	water_shader->setMat4("model", glm::translate(glm::vec3(0.0f, debug.water_height, 0.0f)));
	water_shader->setMat4("view_proj", view_proj);
	water_shader->setInt("reflectTexture", 0);
	water_shader->setInt("refractTexture", 1);
    water_shader->setInt("dudv", 2);
    water_shader->setInt("normal", 3);
    water_shader->setInt("depth", 4);

    water_shader->setVec3("light.position", light.position);
    water_shader->setVec3("light.color", light.color);
    water_shader->setVec3("camera_position", camera.position);
    water_shader->setFloat("time", (float)time.absolute);
    water_shader->setFloat("distortion_strength", debug.distortion_strength);
    water_shader->setFloat("distortion_scale", debug.distortion_scale);
    water_shader->setFloat("refract_strength", debug.refract_strength);

	water_plane.draw();
}

void Scene::Render(void)
{
    // WATER_REFLECTION:
    glBindFramebuffer(GL_FRAMEBUFFER, waterBuffers[WATER_REFLECTION].fbo);
    {
        // unvailable in opengl es 3.1
        // glEnable(GL_CLIP_DISTANCE0);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 800, 600);

        // FIXME: the position of the camera an offset of `dist`
        float dist = 2.0f * (camera.position.y - debug.water_height);
        cameracontroller.InvertPitch();
        cameracontroller.Update(0.0f);
        // camera.position.y -= dist;

        RenderTerrain(glm::vec4(0.0f, 1.0f, 0.0f, -debug.water_height));

        cameracontroller.InvertPitch();
        cameracontroller.Update(0.0f);
        // camera.position.y += dist;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // WATER_REFRACTION:
    glBindFramebuffer(GL_FRAMEBUFFER, waterBuffers[WATER_REFRACTION].fbo);
    {
        // unvailable in opengl es 3.1
        // glEnable(GL_CLIP_DISTANCE0);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, kFramebufferWidth, kFramebufferHeight);
        RenderTerrain(glm::vec4(0.0f, -1.0f, 0.0f, debug.water_height));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // unvailable in opengl es 3.1
    // glDisable(GL_CLIP_DISTANCE0);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, sapp_widthf(), sapp_heightf());

    RenderTerrain(glm::vec4(0.0f, -1.0f, 0.0f, 1000.0f));
    RenderWater();

    const auto view_proj = camera.Projection() * camera.View();
    light_shader->use();
    light_shader->setMat4("model", glm::translate(glm::mat4(1.0f), light.position));
    light_shader->setMat4("view_proj", view_proj);
    light_shader->setVec3("color", light.color);
    light_sphere.draw();
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::SliderFloat("Land Height", &debug.land_scale, 0.0f, 10.0f);
	ImGui::SliderFloat("Water Height", &debug.water_height, 0.0f, 10.0f);
    ImGui::SliderFloat("Distortion Strength", &debug.distortion_strength, 0.0f, 1.0f);
    ImGui::SliderFloat("Distortion Scale", &debug.distortion_scale, 1.0f, 10.0f);
    ImGui::SliderFloat("Refract Strength", &debug.refract_strength, 0.0f, 10.0f);

	ImVec2 size = { 400.0f, 300.0f };
	ImGui::Text("Refraction (fbo.color0)");
	ImGui::Image((ImTextureID)waterBuffers[WATER_REFRACTION].color0, size, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::Text("Reflection (fbo.color0)");
	ImGui::Image((ImTextureID)waterBuffers[WATER_REFLECTION].color0, size);

    ImGui::End();
}