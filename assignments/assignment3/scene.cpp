// batteries
#include "scene.h"
#include "batteries/transform.h"

// imgui
#include "imgui/imgui.h"

// opengl
#include <GLES3/gl3.h>

constexpr int kFramebufferWidth = 800;
constexpr int kFramebufferHeight = 600;

constexpr glm::vec4 light_orbit_radius = {2.0f, 2.0f, -2.0f, 1.0f};

struct FullscreenQuad {
	GLuint vao;
	GLuint vbo;

	void Initialize()
	{
		float quad_vertices[] = {
			// pos (x, y) texcoord (u, v)
			-1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 0.0f,

			-1.0f,  1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f,
			1.0f,  1.0f, 1.0f, 1.0f,
		};

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
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (sizeof(float) * 2));

		glBindVertexArray(0);
	}
} fullscreen_quad;

struct Framebuffer {
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, kFramebufferWidth, kFramebufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position, 0);

		// color attachment
		glGenTextures(1, &normal);
		glBindTexture(GL_TEXTURE_2D, normal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, kFramebufferWidth, kFramebufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal, 0);

		// color attachment
		glGenTextures(1, &albedo);
		glBindTexture(GL_TEXTURE_2D, albedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, kFramebufferWidth, kFramebufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("Not so victorious\n");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
} framebuffer;

struct Material {
	glm::vec3 ambient{ 1.0f }; 
	glm::vec3 diffuse{ 0.5f }; 
	glm::vec3 specular{ 0.5f };
	float shininess = 0.5f;
} material;

struct {
	int width = 1;
} debug;

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/suzanne.obj");
    geometry = std::make_unique<ew::Shader>("assets/shaders/deferred/geometry.vs", "assets/shaders/deferred/geometry.fs");
	lighting = std::make_unique<ew::Shader>("assets/shaders/deferred/blinnphong.vs", "assets/shaders/deferred/blinnphong.fs");
    texture = std::make_unique<ew::Texture>("assets/brick_color.jpg");

    ambient = {
        .intensity = 1.0f,
        .color = { 0.5f, 0.5f, 0.5f },
    };

    light = {
        .brightness = 1.0f,
        .color = { 0.5f, 0.5f, 0.5f },
    };

    framebuffer.Initialize();
    fullscreen_quad.Initialize();
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

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        geometry->use();

        // scene matrices
        geometry->setMat4("model", glm::mat4{1.0f});
        geometry->setMat4("view_proj", view_proj);
        geometry->setVec3("camera_position", camera.position);

        // draw suzanne
		for (auto i = -debug.width; i <= debug.width; i++)
		{
			for (auto j = -debug.width; j <= debug.width; j++)
			{
				geometry->setMat4("model", glm::translate(glm::vec3(i * 3.0f, 0, j * 3.0f)));
				suzanne->draw();
			}
		}
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	{
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
		lighting->setVec3("light.color", light.color);
		lighting->setVec3("light.position", light.position);
		
		// fullscreen quad pipeline:
		glDisable(GL_DEPTH_TEST);
	
		// clear default buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	
		// draw fullscreen quad
		glBindVertexArray(fullscreen_quad.vao);
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebuffer.position);
	
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, framebuffer.normal);
	
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, framebuffer.albedo);
	
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

	ImGui::SliderInt("Width", &debug.width, 0, 50);

	// ImGui::Image((ImTextureID)(intptr_t)framebuffer.position, ImVec2(kFramebufferWidth, kFramebufferHeight)); 
	// ImGui::Image((ImTextureID)(intptr_t)framebuffer.normal, ImVec2(kFramebufferWidth, kFramebufferHeight)); 
	// ImGui::Image((ImTextureID)(intptr_t)framebuffer.albedo, ImVec2(kFramebufferWidth, kFramebufferHeight)); 

    ImGui::End();
}