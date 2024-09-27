#include "scene.h"

#include "batteries/assets.h"

#include "imgui/imgui.h"
#include "sokol/sokol_imgui.h"
#include "sokol/sokol_shape.h"

#include <unordered_map>

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};
static uint8_t file_buffer[1024 * 1024 * 5];

Scene::Scene()
{
    auto load_gradients = [this]()
    {
#define LOAD_GRADIENT(i, filepath)                    \
    gradient.img[i] = sg_alloc_image();               \
    batteries::load_img({.image_id = gradient.img[i], \
                         .path = filepath,            \
                         .buffer = SG_RANGE(file_buffer)})
        LOAD_GRADIENT(0, "assets/transitions/gradient1.png");
        LOAD_GRADIENT(1, "assets/transitions/gradient2.png");
        LOAD_GRADIENT(2, "assets/transitions/gradient3.png");

        // create an image sampler
        gradient.smp = sg_make_sampler({
            .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
            .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
        });

#undef LOAD_GRADIENT
    };

    load_gradients();
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
    const auto view_proj = camera.projection() * camera.view();

    sg_begin_pass({.action = pass_action, .attachments = framebuffer.attachments});

    /* TODO: render the scene */

    sg_end_pass();

    // render framebuffer
    framebuffer.Render(&transition);

    // sg_end_pass();
    // sg_commit();
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);
    ImGui::End();
}