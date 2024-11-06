#include "scene.h"
#include "imgui/imgui.h"

#include "sokol/sokol_imgui.h"

static glm::vec4 light_orbit_radius = {2.0f, 2.0f, -2.0f, 1.0f};

static struct
{
    simgui_image_t depth_buffer;
    simgui_image_t shadow_map;
} debug;

Scene::Scene()
{
    ambient = {
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    light = {
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
    };

    material = {
        .ambient = {0.5f, 0.5f, 0.5f},
        .diffuse = {0.5f, 0.5f, 0.5f},
        .specular = {0.5f, 0.5f, 0.5f},
        .shininess = 128.0f,
    },

    suzanne.Load("assets/suzanne.obj");

    sphere = batteries::CreateSphere(1.0f, 4);
    sphere.transform.scale = {0.25f, 0.25f, 0.25f};

    plane = batteries::CreatePlane(100.0f, 100.0f, 1);
    plane.transform.position = {0.0f, -2.0f, 0.0f};

    cube = batteries::CreateCube(6.0f);
    cube.transform.position = {0.0f, 0.0f, 0.0f};

    // create an sokol-imgui wrapper for the shadow map
    auto ui_smp = sg_make_sampler({
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .label = "ui-sampler",
    });
    debug.depth_buffer = simgui_make_image({
        .image = depthbuffer.depth,
        .sampler = ui_smp,
    });
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    static auto ry = 0.0f;
    ry += time.frame * time.factor;

    // sugar: rotate light
    const auto rym = glm::rotate(ry, glm::vec3(0.0f, 1.0f, 0.0f));
    light.position = rym * light_orbit_radius;

    ambient.direction = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - light.position);
    sphere.transform.position = light.position;
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    // depth pass matrices
    const auto light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
    const auto light_view = glm::lookAt(light.position, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    const auto light_view_proj = light_proj * light_view;

    // initialize uniform data
    const Depth::vs_params_t vs_depth_params = {
        .view_proj = light_view_proj,
        .model = suzanne.transform.matrix(),
    };
    const Shadow::vs_params_t vs_shadow_params = {
        .model = suzanne.transform.matrix(),
        .view_proj = view_proj,
        .light_view_proj = light_view_proj,
    };
    const Shadow::fs_params_t fs_shadow_params = {
        .material = material,
        .light = light,
        .ambient = ambient,
        .camera_position = camera.position,
    };
    const Shadow::vs_params_t vs_dungeon_params = {
        .model = cube.transform.matrix(),
        .view_proj = view_proj,
        .light_view_proj = light_view_proj,
    };
    const Dungeon::fs_params_t fs_dungeon_params = {
        .material = material,
        .light = light,
        .ambient = ambient,
        .camera_position = camera.position,
    };
    const batteries::Gizmo::vs_params_t vs_gizmo_params = {
        .view_proj = view_proj,
        .model = sphere.transform.matrix(),
    };
    const batteries::Gizmo::fs_params_t fs_gizmo_params = {
        .color = light.color,
    };

    sg_begin_pass(&depthbuffer.pass);
    // apply blinnphong pipeline and uniforms
    sg_apply_pipeline(depth.pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_depth_params));
    // render suzanne
    if (suzanne.loaded)
    {
        sg_apply_bindings({
            .vertex_buffers[0] = suzanne.mesh.vertex_buffer,
            .index_buffer = suzanne.mesh.index_buffer,
        });
        sg_draw(0, suzanne.mesh.indices.size(), 1);
    }
    sg_end_pass();

    sg_begin_pass(&framebuffer.pass);
    // apply blinnphong pipeline and uniforms
    sg_apply_pipeline(shadow.pipeline);
    // render suzanne
    if (suzanne.loaded)
    {
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_shadow_params));
        sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_shadow_params));
        sg_apply_bindings({
            .vertex_buffers[0] = suzanne.mesh.vertex_buffer,
            .index_buffer = suzanne.mesh.index_buffer,
            .fs = {
                .images[0] = depthbuffer.depth,
                .samplers[0] = depthbuffer.sampler,
            },
        });
        sg_draw(0, suzanne.mesh.indices.size(), 1);
    }

    sg_apply_pipeline(dungeon.pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_dungeon_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_dungeon_params));
    sg_apply_bindings({
        .vertex_buffers[0] = cube.mesh.vertex_buffer,
        .index_buffer = cube.mesh.index_buffer,
        .fs = {
            .images[0] = depthbuffer.depth,
            .samplers[0] = depthbuffer.sampler,
        },
    });
    sg_draw(0, cube.mesh.indices.size(), 1);

    // render light sources
    sg_apply_pipeline(gizmo.pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_gizmo_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_gizmo_params));
    sg_apply_bindings({
        .vertex_buffers[0] = sphere.mesh.vertex_buffer,
        .index_buffer = sphere.mesh.index_buffer,
    });
    sg_draw(0, sphere.mesh.indices.size(), 1);
    sg_end_pass();

    // render framebuffer
    sg_begin_pass(&pass);
    sg_apply_pipeline(framebuffer.pipeline);
    sg_apply_bindings(&framebuffer.bindings);
    sg_draw(0, 6, 1);
}

void Scene::Debug(void)
{
    auto window_size = ImGui::GetWindowSize();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);

    if (ImGui::CollapsingHeader("Camera"))
    {
        ImGui::Text("Position: %.2f, %.2f, %.2f", camera.position[0], camera.position[1], camera.position[2]);
    }

    if (ImGui::CollapsingHeader("Ambient"))
    {
        ImGui::SliderFloat("Intensity", &ambient.intensity, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color", &ambient.color[0]);
    }
    if (ImGui::CollapsingHeader("Light"))
    {
        ImGui::SliderFloat("Brightness", &light.brightness, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color", &light.color[0]);
    }
    ImGui::End();

    ImGui::Begin("Offscreen Render");
    ImGui::BeginChild("Depth Buffer");
    ImGui::Image(simgui_imtextureid(debug.depth_buffer), window_size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::EndChild();
    ImGui::BeginChild("Shadow Map");
    ImGui::Image(simgui_imtextureid(debug.shadow_map), window_size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::EndChild();
    ImGui::End();
}