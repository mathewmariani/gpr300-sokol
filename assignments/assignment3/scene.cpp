#include "scene.h"

// batteries
#include "batteries/math.h"

// ew
#include "ew/procGen.h"

// imgui
#include "imgui/imgui.h"

// opengl
#include <GLES3/gl3.h>

constexpr int kFramebufferWidth = 800;
constexpr int kFramebufferHeight = 600;

constexpr float orbit_radius = 2.0f;
constexpr glm::vec4 light_orbit_radius = {2.0f, 4.0f, -4.0f, 1.0f};

struct FullscreenQuad
{
    GLuint vao;
    GLuint vbo;

    void Initialize()
    {
        // clang-format off
        float quad_vertices[] = {
            // pos (x, y) texcoord (u, v)
            -1.0f,  1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f,  1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f,  1.0f, 1.0f, 1.0f,
        };
        // clang-format on

        // initialize fullscreen quad, buffer object
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        // bind vao, and vbo
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // buffer data to vbo
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

        // positions and texcoords
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

        glBindVertexArray(0);
    }
} fullscreen_quad;

struct Framebuffer
{
    GLuint fbo;
    GLuint position;
    GLuint normal;
    GLuint albedo;
    GLuint depth;

    void Initialize()
    {
        // initialize framebuffer
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // color attachment
        glGenTextures(1, &position);
        glBindTexture(GL_TEXTURE_2D, position);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, kFramebufferWidth, kFramebufferHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position, 0);

        // color attachment
        glGenTextures(1, &normal);
        glBindTexture(GL_TEXTURE_2D, normal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, kFramebufferWidth, kFramebufferHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal, 0);

        // color attachment
        glGenTextures(1, &albedo);
        glBindTexture(GL_TEXTURE_2D, albedo);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, kFramebufferWidth, kFramebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedo, 0);

        // Create depth texture
        glGenTextures(1, &depth);
        glBindTexture(GL_TEXTURE_2D, depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, kFramebufferWidth, kFramebufferHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

        GLenum array[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
        glDrawBuffers(3, array);

        // check completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Not so victorious\n");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} framebuffer;

struct Material
{
    glm::vec3 ambient{1.0f};
    glm::vec3 diffuse{0.5f};
    glm::vec3 specular{0.5f};
    float shininess = 0.5f;
} material;

struct
{
    int width = 1;
} debug;

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/suzanne.obj");
    geometry = std::make_unique<ew::Shader>("assets/shaders/deferred/geometry.vs", "assets/shaders/deferred/geometry.fs");
    lighting = std::make_unique<ew::Shader>("assets/shaders/deferred/blinnphong.vs", "assets/shaders/deferred/blinnphong.fs");
    lightsphere = std::make_unique<ew::Shader>("assets/shaders/deferred/light.vs", "assets/shaders/deferred/light.fs");
    texture = std::make_unique<ew::Texture>("assets/brick_color.jpg");

    sphere.load(ew::createSphere(0.25f, 4));

    ambient = {
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    framebuffer.Initialize();
    fullscreen_quad.Initialize();

    InitializeInstanceData();
}

Scene::~Scene()
{
}

void Scene::InitializeInstanceData(void)
{
    auto width = debug.width;
    auto size = (width - (-width) + 1) * (width - (-width) + 1);
    model_instances.resize(size);
    light_instances.resize(size);

    auto i = 0;
    for (auto x = -debug.width; x <= debug.width; x++)
    {
        for (auto y = -debug.width; y <= debug.width; y++, i++)
        {
            const auto position = glm::vec3(x * 3.0f, 0, y * 3.0f);
            const auto orbit = batteries::random_point_on_sphere();

            // light instances
            light_instances[i] = {
                .color = batteries::random_color(),
                .position = glm::vec4(position, 1.0f) + orbit  * orbit_radius,
            };

            // model instances
            model_instances[i] = batteries::random_model_matrix(position);
        }
    }
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        glViewport(0, 0, kFramebufferWidth, kFramebufferHeight);

        geometry->use();

        // scene matrices
        geometry->setMat4("model", glm::mat4{1.0f});
        geometry->setMat4("view_proj", view_proj);
        geometry->setVec3("camera_position", camera.position);

        // draw suzanne
        auto i = 0;
        for (auto x = -debug.width; x <= debug.width; x++)
        {
            for (auto y = -debug.width; y <= debug.width; y++, i++)
            {
                geometry->setMat4("model", model_instances[i]);

                // Draw the object
                suzanne->draw();
            }
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    { // render fullscreen quad
        lighting->use();
        lighting->setInt("g_position", 0);
        lighting->setInt("g_normal", 1);
        lighting->setInt("g_albedo", 2);

        lighting->setVec3("camera_position", camera.position);

        // material properties
        lighting->setVec3("material.ambient", material.ambient);
        lighting->setVec3("material.diffuse", material.diffuse);
        lighting->setVec3("material.specular", material.specular);
        lighting->setFloat("material.shininess", material.shininess);

        // ambient light
        lighting->setFloat("ambient.intensity", ambient.intensity);
        lighting->setVec3("ambient.color", ambient.color);

        // point light
        auto count = light_instances.size() > 100 ? 100 : light_instances.size();
        for (auto i = 0; i < count; i++)
        {
            lighting->setVec3("lights[" + std::to_string(i) + "].color", light_instances[i].color);
            lighting->setVec3("lights[" + std::to_string(i) + "].position", light_instances[i].position);
        }

        // fullscreen quad pipeline:
        glDisable(GL_DEPTH_TEST);

        // clear default buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, sapp_widthf(), sapp_heightf());

        // draw fullscreen quad
        glBindVertexArray(fullscreen_quad.vao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer.position);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, framebuffer.normal);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, framebuffer.albedo);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
    }

    { // render light sources
        glEnable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(0, 0, 800, 600, 0, 0, sapp_widthf(), sapp_heightf(), GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        lightsphere->use();

        auto i = 0;
        for (auto x = -debug.width; x <= debug.width; x++)
        {
            for (auto y = -debug.width; y <= debug.width; y++, i++)
            {
                // scene matrices
                lightsphere->setMat4("model", glm::translate(light_instances[i].position));
                lightsphere->setMat4("view_proj", view_proj);

                lightsphere->setVec3("color", light_instances[i].color);

                sphere.draw();
            }
        }
    }
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    if (ImGui::SliderInt("Width", &debug.width, 1, 100))
    {
        InitializeInstanceData();
    }

    if (ImGui::CollapsingHeader("Geometry Buffer"))
    {
        ImVec2 uv_min(0.0f, 1.0f);
        ImVec2 uv_max(1.0f, 0.0f);

        ImGui::Text("Albedo:");
        ImGui::Image((ImTextureID)(intptr_t)framebuffer.albedo, ImVec2(200, 150), uv_min, uv_max);

        ImGui::Text("Position:");
        ImGui::Image((ImTextureID)(intptr_t)framebuffer.position, ImVec2(200, 150), uv_min, uv_max);

        ImGui::Text("Normal:");
        ImGui::Image((ImTextureID)(intptr_t)framebuffer.normal, ImVec2(200, 150), uv_min, uv_max);

        ImGui::Text("Depth:");
        ImGui::Image((ImTextureID)(intptr_t)framebuffer.depth, ImVec2(200, 150), uv_min, uv_max);
    }
    ImGui::End();
}