// batteries
#include "scene.h"
#include "batteries/transform.h"

// imgui
#include "imgui/imgui.h"

// opengl
#include <GLES3/gl3.h>

constexpr int kFramebufferWidth = 800;
constexpr int kFramebufferHeight = 600;

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
	GLuint color0;
	GLuint color1;
	GLuint color2;
	GLuint depth;

	void Initialize()
	{
		// initialize framebuffer
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// color attachment
		glGenTextures(1, &color0);
		glBindTexture(GL_TEXTURE_2D, color0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, kFramebufferWidth, kFramebufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color0, 0);

		// color attachment
		glGenTextures(1, &color1);
		glBindTexture(GL_TEXTURE_2D, color1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, kFramebufferWidth, kFramebufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, color1, 0);

		// color attachment
		glGenTextures(1, &color2);
		glBindTexture(GL_TEXTURE_2D, color2);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, kFramebufferWidth, kFramebufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, color2, 0);

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

void post_process(ew::Shader* shader)
{
	shader->use();
	shader->setInt("g_albedo", 0);
	shader->setInt("g_position", 1);
	shader->setInt("g_normal", 2);

	// fullscreen quad pipeline:
	glDisable(GL_DEPTH_TEST);

	// clear default buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	// draw fullscreen quad
	glBindVertexArray(fullscreen_quad.vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer.color0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, framebuffer.color1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, framebuffer.color2);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/suzanne.obj");
    geometry = std::make_unique<ew::Shader>("assets/shaders/deferred/geometry.vs", "assets/shaders/deferred/geometry.fs");
	lighting = std::make_unique<ew::Shader>("assets/shaders/deferred/blinnphong.vs", "assets/shaders/deferred/blinnphongt.fs");
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
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        // set bindings
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->getID());

        geometry->use();

        // samplers
        geometry->setInt("texture0", 0);

        // scene matrices
        geometry->setMat4("model", glm::mat4{1.0f});
        geometry->setMat4("view_proj", view_proj);
        geometry->setVec3("camera_position", camera.position);

        // draw suzanne
		for (auto i = -1; i <= 1; i++)
		{
			for (auto j = -1; j <= 1; j++)
			{
				geometry->setMat4("model", glm::translate(glm::vec3(i * 2.0f, 0, j * 2.0f)));
				suzanne->draw();
			}
		}
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    post_process(lighting.get());
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

	ImGui::Image((ImTextureID)(intptr_t)framebuffer.color0, ImVec2(kFramebufferWidth, kFramebufferHeight)); 
	ImGui::Image((ImTextureID)(intptr_t)framebuffer.color1, ImVec2(kFramebufferWidth, kFramebufferHeight)); 
	ImGui::Image((ImTextureID)(intptr_t)framebuffer.color2, ImVec2(kFramebufferWidth, kFramebufferHeight)); 

    ImGui::End();
}