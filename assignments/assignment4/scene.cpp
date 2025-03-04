#include "scene.h"

#include "catnumrollspline.h"

// batteries
#include "batteries/transform.h"

// glm
#include "glm/glm.hpp"

// imgui
#include "imgui/imgui.h"

// opengl
#include <GLES3/gl3.h>

// sokol
#include <vector>

#include "sokol/sokol_app.h"

constexpr glm::vec3 color_1{1.00f, 1.00f, 1.00f};
constexpr glm::vec3 color_2{0.20f, 0.59f, 0.85f};

std::vector<glm::vec3> user_points;
std::vector<glm::vec3> curve_points;

// buffer objects
GLuint vao, curve_vbo, point_vbo;

static glm::vec3 woldToScreenPos(const glm::mat4& proj, const glm::vec3& pos)
{
    auto ret = glm::vec4(pos.x, pos.y, pos.z, 0.0f) * proj;
    ret.x = (ret.x - 1);
    ret.y = -(ret.y + 1);

    return (glm::vec3)ret;
}

Scene::Scene()
{
    splines = std::make_unique<ew::Shader>("assets/shaders/spline.vs", "assets/shaders/spline.fs");

    // make stuff more easily visible
    // glPointSize(10);
    glLineWidth(10);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &point_vbo);
    glGenBuffers(1, &curve_vbo);
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
    float width = sapp_widthf();
    float height = sapp_heightf();
    const auto proj = glm::ortho(0.0f, width, height, 0.0f, 1.0f, -1.0f);
    const auto model = glm::mat4(1.0f);

    // clear the buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (user_points.size() <= 1)
    {
        return;
    }

    // attach shader
    splines->use();
    splines->setMat4("proj", proj);
    splines->setMat4("model", model);

    //>> catmullroll curve
    splines->setVec3("color", color_1);

    // setup attribute buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, curve_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // draw our points
    glDrawArrays(GL_LINE_STRIP, 0, curve_points.size());
    glDisableVertexAttribArray(0);

    //>> user points
    splines->setVec3("color", color_2);

    // setup attribute buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // draw our points
    glDrawArrays(GL_POINTS, 0, user_points.size());
    glDisableVertexAttribArray(0);
}

void Scene::Event(const sapp_event* event)
{
    // batteries::Scene::Event(event);

    const auto proj = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

    if (event->type == SAPP_EVENTTYPE_MOUSE_DOWN)
    {
        auto xpos = (float)event->mouse_x;
        auto ypos = (float)event->mouse_y;

        // push our point back
        user_points.emplace_back(xpos, ypos, 0.0f);

        // FIXME: wasteful.
        glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * user_points.size(), &user_points[0], GL_STATIC_DRAW);

        if (user_points.size() <= 1)
        {
            return;
        }

        curve_points.clear();
        curve_points = CatnumrollSpline(user_points, 5);

        // rebuild our vbo
        // FIXME: wasteful.
        glBindBuffer(GL_ARRAY_BUFFER, curve_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * curve_points.size(), &curve_points[0], GL_STATIC_DRAW);
    }
}

void Scene::Debug(void)
{
    // ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // ImGui::Checkbox("Paused", &time.paused);
    // ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    // ImGui::End();
}