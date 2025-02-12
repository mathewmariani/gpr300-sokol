#include "scene.h"

// batteries
#include "batteries/materials.h"
#include "batteries/transform.h"

// imgui
#include "imgui/imgui.h"

// opengl
#include <GLES3/gl3.h>

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};

struct Material {
	glm::vec3 ambient{ 1.0f }; 
	glm::vec3 diffuse{ 0.5f }; 
	glm::vec3 specular{ 0.5f };
	float shininess = 0.5f;
} material;

struct {
    float cutoff = 0.0f;
    glm::vec3 color{ 1.0f, 0.0f, 1.0f };
} transition_fs_params;

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
	GLuint depth;

	void Initialize()
	{
		// initialize framebuffer
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// color attachment
		glGenTextures(1, &color0);
		glBindTexture(GL_TEXTURE_2D, color0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color0, 0);

		// Create depth texture
		glGenTextures(1, &depth);
		glBindTexture(GL_TEXTURE_2D, depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

		// check completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("Not so victorious\n");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
} framebuffer;

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/suzanne.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/blinnphong.vs", "assets/shaders/blinnphong.fs");
    transition = std::make_unique<ew::Shader>("assets/shaders/transition.vs", "assets/shaders/transition.fs");

    gradients.push_back(std::make_unique<ew::Texture>("assets/transitions/gradient1.png"));
    gradients.push_back(std::make_unique<ew::Texture>("assets/transitions/gradient2.png"));
    gradients.push_back(std::make_unique<ew::Texture>("assets/transitions/gradient3.png"));

    fullscreen_quad.Initialize();
    framebuffer.Initialize();

    ambient = {
        .intensity = 1.0f,
        .color = { 0.5f, 0.5f, 0.5f },
    };

    light = {
        .brightness = 1.0f,
        .color = { 0.5f, 0.5f, 0.5f },
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

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        // set bindings
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->getID());

        blinnphong->use();

        // samplers
        blinnphong->setInt("texture0", 0);

        // scene matrices
        blinnphong->setMat4("model", glm::mat4{1.0f});
        blinnphong->setMat4("view_proj", view_proj);
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

        // draw suzanne
        suzanne->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    transition->use();
	transition->setInt("gradient", 0);
    transition->setFloat("transition.cutoff", transition_fs_params.cutoff);
    transition->setVec3("transition.color", transition_fs_params.color);

	// fullscreen quad pipeline:
	glDisable(GL_DEPTH_TEST);

	// clear default buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	// draw fullscreen quad
	glBindVertexArray(fullscreen_quad.vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gradients[0]->getID());
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::Separator();
    ImGui::SliderFloat("Cutoff", &transition_fs_params.cutoff, 0.0f, 1.0f);
    ImGui::ColorEdit3("Fadeout Color", &transition_fs_params.color[0]);

    ImGui::End();
}