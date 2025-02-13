#include "scene.h"
#include "batteries/assets.h"
#include "imgui/imgui.h"

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};

static sg_sampler sampler;

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
    };

    transition.settings.fs_params = {
        .color = {0.00f, 0.31f, 0.85f},
        .cutoff = 0.0f,
    };

    suzanne.Load("assets/suzanne.obj");
    gradients[0].Load("assets/transitions/gradient1.png");
    gradients[1].Load("assets/transitions/gradient2.png");
    gradients[2].Load("assets/transitions/gradient3.png");

    sphere = batteries::CreateSphere(1.0f, 4);
    sphere.transform.scale = {0.25f, 0.25f, 0.25f};

    sampler = sg_make_sampler({
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
    });
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    static auto ry = 0.0f;
    ry += time.frame;

    // sugar: rotate light
    const auto rym = glm::rotate(ry, glm::vec3(0.0f, 1.0f, 0.0f));
    light.position = rym * light_orbit_radius;

    sphere.transform.position = light.position;
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    // initialize uniform data
    const BlinnPhong::vs_params_t vs_blinnphong_params = {
        .view_proj = view_proj,
        .model = suzanne.transform.matrix(),
    };
    const BlinnPhong::fs_params_t fs_blinnphong_params = {
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

    sg_begin_pass(&framebuffer.pass);
    // apply blinnphong pipeline and uniforms
    sg_apply_pipeline(blinnphong.pipeline);
    sg_apply_uniforms(0, SG_RANGE(vs_blinnphong_params));
    sg_apply_uniforms(1, SG_RANGE(fs_blinnphong_params));
    // render suzanne
    if (suzanne.loaded)
    {
        sg_apply_bindings({
            .vertex_buffers[0] = suzanne.mesh.vertex_buffer,
            .index_buffer = suzanne.mesh.index_buffer,
        });
        sg_draw(0, suzanne.mesh.indices.size(), 1);
    }

    // render light sources
    sg_apply_pipeline(gizmo.pipeline);
    sg_apply_uniforms(0, SG_RANGE(vs_gizmo_params));
    sg_apply_uniforms(1, SG_RANGE(fs_gizmo_params));
    sg_apply_bindings({
        .vertex_buffers[0] = sphere.mesh.vertex_buffer,
        .index_buffer = sphere.mesh.index_buffer,
    });
    sg_draw(0, sphere.mesh.indices.size(), 1);
    sg_end_pass();

    // render framebuffer
    sg_begin_pass(&pass);
    sg_apply_pipeline(transition.pipeline);
    sg_apply_uniforms(0, SG_RANGE(transition.settings.fs_params));
    sg_apply_bindings({
        .vertex_buffers[0] = framebuffer.vertex_buffer,
        .images[0] = gradients[0].image,
        .samplers[0] = sampler,
    });
    sg_draw(0, 6, 1);
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);
    ImGui::DragFloat("Cutoff", &transition.settings.fs_params.cutoff, 0.01f, 0.00f, 1.00f);
    ImGui::ColorEdit3("Fadeout Color", &transition.settings.fs_params.color[0]);
    // if (ImGui::Button("Gradient 1"))
    // {
    //     transition.bindings.fs.images[0] = gradients[0];
    // }
    // if (ImGui::Button("Gradient 2"))
    // {
    //     transition.bindings.fs.images[0] = gradients[1];
    // }
    // if (ImGui::Button("Gradient 3"))
    // {
    //     transition.bindings.fs.images[0] = gradients[2];
    // }
    ImGui::End();
}