#include "scene.h"

// imgui
#include "imgui/imgui.h"

// batteries
#include "batteries/materials.h"
#include "batteries/math.h"

// ew
#include "ew/procGen.h"

// opengl
#if defined(SOKOL_GLCORE)
#include "gl3w/gl3w.h"
#else
#include <GLES3/gl3.h>
#endif

static glm::vec4 suzanne_orbit_radius = {2.0f, 0.0f, -2.0f, 1.0f};

struct Material
{
    glm::vec3 ambient{1.0f};
    glm::vec3 diffuse{0.5f};
    glm::vec3 specular{0.5f};
    float shininess = 0.5f;
} material;

struct Depthbuffer
{
    GLuint fbo;
    GLuint depth;

    void Initialize()
    {
        glGenFramebuffers(1, &fbo);

        glGenTextures(1, &depth);
        glBindTexture(GL_TEXTURE_2D, depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

        glClear(GL_DEPTH_BUFFER_BIT);
        glDrawBuffers(0, nullptr);
        glReadBuffer(GL_NONE);

        // check completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Not so victorious\n");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} depthbuffer;

struct
{
    float bias = 0.005f;
    bool cull_front = false;
    bool use_pcf = false;
    float farz = 2.5f;
    float nearz = 0.01f;
    glm::mat4 suzanne_mat = glm::mat4(1.0f);
} debug;

Scene::Scene()
{
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/shadow.vs", "assets/shaders/shadow.fs");
    depth = std::make_unique<ew::Shader>("assets/shaders/depth.vs", "assets/shaders/depth.fs");
    snow = std::make_unique<ew::Shader>("assets/shaders/snow.vs", "assets/shaders/snow.fs");

    suzanne = std::make_unique<ew::Model>("assets/suzanne.obj");
    snow_texture = std::make_unique<ew::Texture>("assets/snow.png");

    ambient = {
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    light = {
        .brightness = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
        .position = {0.0f, -2.0f, 0.0f},
    };

    depthbuffer.Initialize();

    plane.load(ew::createPlane(50.0f, 50.0f, 1000));
    // plane.load(ew::createCube(15.0f));
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    const auto rym = glm::rotate((float)time.absolute, glm::vec3(0.0f, 1.0f, 0.0f));
    debug.suzanne_mat = glm::translate(glm::vec3(rym * suzanne_orbit_radius));
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    const auto light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, debug.nearz, debug.farz);
    const auto light_view = glm::lookAt(light.position, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    const auto light_view_proj = light_proj * light_view;

    // draw the scene only using the depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthbuffer.fbo);
    {
        glEnable(GL_CULL_FACE);
        glCullFace(debug.cull_front ? GL_FRONT : GL_BACK);
        glEnable(GL_DEPTH_TEST);

        glViewport(0, 0, 1024, 1024);

        depth->use();

        // scene matrices
        depth->setMat4("model", debug.suzanne_mat);
        depth->setMat4("light_view_proj", light_view_proj);

        // draw scene
        suzanne->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    // reset viewport
    glViewport(0, 0, sapp_width(), sapp_height());

    // set bindings
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthbuffer.depth);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, snow_texture->getID());

    blinnphong->use();

    // samplers
    blinnphong->setInt("shadow_map", 0);

    // scene matrices
    blinnphong->setMat4("model", debug.suzanne_mat);
    blinnphong->setMat4("view_proj", view_proj);
    blinnphong->setMat4("light_view_proj", light_view_proj);
    blinnphong->setVec3("camera_position", camera.position);

    // material properties
    blinnphong->setVec3("material.ambient", material.ambient);
    blinnphong->setVec3("material.diffuse", material.diffuse);
    blinnphong->setVec3("material.specular", material.specular);
    blinnphong->setFloat("material.shininess", material.shininess);

    // ambient light
    blinnphong->setFloat("ambient.intensity", ambient.intensity);
    blinnphong->setVec3("ambient.color", ambient.color);

    // point light
    blinnphong->setVec3("light.color", light.color);
    blinnphong->setVec3("light.position", light.position);

    blinnphong->setFloat("bias", debug.bias);
    blinnphong->setInt("use_pcf", debug.use_pcf);

    // draw suzanne
    suzanne->draw();

    snow->use();

    // samplers
    snow->setInt("shadow_map", 0);
    snow->setInt("snow_texture", 1);

    // scene matrices
    snow->setMat4("model", glm::translate(glm::vec3(0.0f, -1.0f, 0.0f)));
    snow->setMat4("view_proj", view_proj);
    snow->setMat4("light_view_proj", light_view_proj);
    snow->setVec3("camera_position", camera.position);

    // point light
    snow->setVec3("light.color", light.color);
    snow->setVec3("light.position", light.position);

    snow->setFloat("bias", debug.bias);
    snow->setInt("use_pcf", debug.use_pcf);

    plane.draw();
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::SeparatorText("Shadow Mapping");
    ImGui::Checkbox("cull_front", &debug.cull_front);
    ImGui::Checkbox("use_pcf", &debug.use_pcf);
    ImGui::SliderFloat("Bias", &debug.bias, 0.05f, 0.00f);

    ImGui::SliderFloat("Near", &debug.nearz, 0.1f, 10.0f);
    ImGui::SliderFloat("Far", &debug.farz, 0.1f, 10.0f);

    ImGui::SeparatorText("Material");
    ImGui::SliderFloat3("Ambient", &material.ambient[0], 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", &material.diffuse[0], 0.0f, 1.0f);
    ImGui::SliderFloat3("Specular", &material.specular[0], 0.0f, 1.0f);
    ImGui::SliderFloat("Shininess", &material.shininess, 0.0f, 1.0f);

    ImGui::SeparatorText("Ambient");
    ImGui::SliderFloat("Intensity", &ambient.intensity, 0.0f, 1.0f);
    ImGui::ColorEdit3("Color", &ambient.color[0]);

    ImGui::Image((ImTextureID)(intptr_t)depthbuffer.depth, ImVec2(1024, 1024));

    ImGui::End();
}