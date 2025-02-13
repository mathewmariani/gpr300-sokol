#include "scene.h"

// batteries
#include "batteries/math.h"

// imgui
#include "imgui/imgui.h"
#include "sokol/sokol_imgui.h"

static constexpr int max_instances = 64;
static BlinnPhong::my_light_t instance_light_data;
static constexpr glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};
sg_attachments gizmo_attachments;
sg_pass_action gizmo_pass_action;
sg_buffer instance_buffer;
static glm::mat4 instance_data[max_instances];
static auto num_instances = 64;

static struct
{
    ImTextureID color_img;
    ImTextureID position_img;
    ImTextureID normal_img;
    ImTextureID depth_img;
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

    instance_buffer = sg_make_buffer({
        .type = SG_BUFFERTYPE_VERTEXBUFFER,
        .data = SG_RANGE(instance_data),
    });
}

Scene::Scene()
{
    init_instance_data();

    ambient = {
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    light = {
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
    };

    gizmo_pass_action = {
        .colors[0].load_action = SG_LOADACTION_DONTCARE,
        .depth.load_action = SG_LOADACTION_DONTCARE,
    };
    gizmo_attachments = sg_make_attachments({
        .colors[0].image = framebuffer.color,
        .depth_stencil.image = geometrybuffer.depth_img,
        .label = "gizmo-attachment",
    });

    suzanne.Load("assets/suzanne.obj");

    sphere = batteries::CreateSphere(1.0f, 4);
    sphere.transform.scale = {0.25f, 0.25f, 0.25f};

    debug.color_img = simgui_imtextureid(geometrybuffer.color_img);
    debug.position_img = simgui_imtextureid(geometrybuffer.position_img);
    debug.normal_img = simgui_imtextureid(geometrybuffer.normal_img);
    debug.depth_img = simgui_imtextureid(geometrybuffer.depth_img);
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

    // initialize uniform data
    const BlinnPhong::fs_params_t fs_blinnphong_params = {
        .camera_position = camera.position,
        .lights = instance_light_data,
        .ambient = ambient,
        .num_instances = num_instances,
    };
    const Geometry::vs_params_t vs_geometry_params = {
        .view_proj = view_proj,
    };

    sg_begin_pass(&geometrybuffer.pass);
    sg_apply_pipeline(geometry.pipeline);
    sg_apply_uniforms(0, SG_RANGE(vs_geometry_params));
    // render suzanne
    if (suzanne.loaded)
    {
        sg_apply_bindings({
            .vertex_buffers = {
                [0] = suzanne.mesh.vertex_buffer,
                [1] = instance_buffer,
            },
            .index_buffer = suzanne.mesh.index_buffer,
        });
        sg_draw(0, suzanne.mesh.indices.size(), num_instances);
    }
    sg_end_pass();

    sg_begin_pass(&framebuffer.pass);
    sg_apply_pipeline(blinnphong.pipeline);
    sg_apply_uniforms(0, SG_RANGE(fs_blinnphong_params));
    sg_apply_bindings({
        .vertex_buffers[0] = framebuffer.vertex_buffer,
        .images = {
            [0] = geometrybuffer.position_img,
            [1] = geometrybuffer.normal_img,
            [2] = geometrybuffer.color_img,
        },
        .samplers[0] = geometrybuffer.sampler,
    });
    sg_draw(0, 6, 1);
    sg_end_pass();

    sg_begin_pass({.action = gizmo_pass_action, .attachments = gizmo_attachments});
    // render light sources
    sg_apply_pipeline(gizmo.pipeline);
    for (auto i = 0; i < num_instances; i++)
    {
        sphere.transform.position = glm::vec3(instance_light_data.position[i]);
        // initialize uniform data
        const batteries::Gizmo::vs_params_t vs_gizmo_params = {
            .view_proj = view_proj,
            .model = sphere.transform.matrix(),
        };
        const batteries::Gizmo::fs_params_t fs_gizmo_params = {
            .color = instance_light_data.color[i],
        };
        sg_apply_uniforms(0, SG_RANGE(vs_gizmo_params));
        sg_apply_uniforms(1, SG_RANGE(fs_gizmo_params));
        sg_apply_bindings({
            .vertex_buffers[0] = sphere.mesh.vertex_buffer,
            .index_buffer = sphere.mesh.index_buffer,
        });
        sg_draw(0, sphere.mesh.indices.size(), 1);
    }
    sg_end_pass();

    // render framebuffer
    sg_begin_pass(&pass);
    sg_apply_pipeline(framebuffer.pipeline);
    sg_apply_bindings(&framebuffer.bindings);
    sg_draw(0, 6, 1);
}

void Scene::Debug(void)
{
    auto size = ImGui::GetWindowSize();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    if (ImGui::CollapsingHeader("Camera"))
    {
        ImGui::Text("Position: %.2f, %.2f, %.2f", camera.position[0], camera.position[1], camera.position[2]);
    }
    ImGui::End();

    ImGui::Begin("Offscreen Render");
    ImGui::BeginChild("Offscreen Render");
    ImGui::Image(debug.position_img, size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::Image(debug.normal_img, size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::Image(debug.color_img, size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::Image(debug.depth_img, size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::EndChild();
    ImGui::End();
}