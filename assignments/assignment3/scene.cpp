#include "scene.h"

#include "batteries/math.h"

#include "imgui/imgui.h"
#include "sokol/sokol_imgui.h"

#include <unordered_map>

static constexpr int max_instances = 64;
static BlinnPhong::my_light_t instance_light_data;
static constexpr glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};
static uint8_t file_buffer[1024 * 1024 * 5];
sg_attachments gizmo_attachments;

static glm::mat4 instance_data[max_instances];

static struct
{
    simgui_image_t color_img;
    simgui_image_t position_img;
    simgui_image_t normal_img;
    simgui_image_t depth_img;
} debug;

static void init_instance_data(void)
{
    const auto offset = 5.0f;
    const auto radius = 3.0f;
    for (int i = 0, x = 0, y = 0, dx = 0, dy = 0; i < max_instances; i++, x += dx, y += dy)
    {
        // transform
        const auto position = glm::vec3(x, 0.0f, y) * offset;

        // random color
        const auto r = static_cast<float>(((rand() % 100) / 200.0f) + 0.5f);
        const auto g = static_cast<float>(((rand() % 100) / 200.0f) + 0.5f);
        const auto b = static_cast<float>(((rand() % 100) / 200.0f) + 0.5f);

        // suzanne
        glm::mat4 *inst = &instance_data[i];
        *inst = glm::translate(position);

        // lights
        const auto orbit = batteries::random_point_on_sphere() * radius;
        instance_light_data.color[i] = {r, g, b, 1.0f};
        instance_light_data.position[i] = glm::vec4(position, 1.0f) + orbit;

        // at a corner?
        if (abs(x) == abs(y))
        {
            if (x >= 0)
            {
                // top-right corner: start a new ring
                if (y >= 0)
                {
                    x += 1;
                    y += 1;
                    dx = 0;
                    dy = -1;
                }
                // bottom-right corner
                else
                {
                    dx = -1;
                    dy = 0;
                }
            }
            else
            {
                // top-left corner
                if (y >= 0)
                {
                    dx = +1;
                    dy = 0;
                }
                // bottom-left corner
                else
                {
                    dx = 0;
                    dy = +1;
                }
            }
        }
    }
}

Scene::Scene()
    : gbuffer(800, 600), blinnphong(gbuffer)
{
    init_instance_data();

    ambient = (batteries::ambient_t){
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    light = (batteries::light_t){
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
    };

    gizmo_attachments = sg_make_attachments({
        .colors[0].image = framebuffer.color,
        .depth_stencil.image = gbuffer.depth_img,
        .label = "gizmo-attachment",
    });

    suzanne.Load("assets/suzanne.obj");

    auto dbg_smp = sg_make_sampler({
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .label = "ui-sampler",
    });

    debug.color_img = simgui_make_image({
        .image = gbuffer.color_img,
        .sampler = dbg_smp,
    });
    debug.position_img = simgui_make_image({
        .image = gbuffer.position_img,
        .sampler = dbg_smp,
    });
    debug.normal_img = simgui_make_image({
        .image = gbuffer.normal_img,
        .sampler = dbg_smp,
    });
    debug.depth_img = simgui_make_image({
        .image = gbuffer.depth_img,
        .sampler = dbg_smp,
    });
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
    static auto num_instances = 9;
    const auto view_proj = camera.projection() * camera.view();

    // initialize uniform data
    const BlinnPhong::fs_params_t fs_params = {
        .camera_position = camera.position,
        .lights = instance_light_data,
        .ambient = ambient,
        .num_instances = num_instances,
    };
    // const batteries::Gizmo::vs_params_t vs_gizmo_params = {
    //     .view_proj = view_proj,
    //     .model = glm::translate(glm::mat4(1.0f), light.position),
    // };
    // const batteries::Gizmo::fs_params_t fs_gizmo_params = {
    //     .color = light.color,
    // };
    const batteries::Skybox::vs_params_t vs_skybox_params = {
        .view_proj = camera.projection() * glm::mat4(glm::mat3(camera.view())),
    };

    const Geometry::vs_params_t vs_geometry_params = {
        .view_proj = view_proj,
    };

    sg_begin_pass(&gbuffer.pass);
    geometry.Render(vs_geometry_params, suzanne, num_instances);
    sg_end_pass();

    sg_begin_pass({.action = pass_action, .attachments = framebuffer.attachments});
    blinnphong.Render(fs_params);
    sg_end_pass();

    sg_begin_pass({.action = deferred_action, .attachments = gizmo_attachments});
    // gizmo.Render(vs_gizmo_params, fs_gizmo_params);
    skybox.Render(vs_skybox_params);
    sg_end_pass();

    framebuffer.Render();

    // sg_end_pass();
    // sg_commit();
}

void Scene::Debug(void)
{
    auto size = ImGui::GetWindowSize();

    ImGui::Begin("Offscreen Render");
    ImGui::BeginChild("Offscreen Render");
    ImGui::Image(simgui_imtextureid(debug.position_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::Image(simgui_imtextureid(debug.normal_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::Image(simgui_imtextureid(debug.color_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::Image(simgui_imtextureid(debug.depth_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::EndChild();
    ImGui::End();
}