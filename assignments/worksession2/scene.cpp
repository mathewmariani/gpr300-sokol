#include "scene.h"

// batteries
#include "batteries/assets.h"

// imgui
#include "imgui/imgui.h"

static struct
{
    float scale = 10.0f;
} debug;

sg_sampler sampler;

Scene::Scene()
{
    sampler = sg_make_sampler({
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .wrap_u = SG_WRAP_CLAMP_TO_BORDER,
        .wrap_v = SG_WRAP_CLAMP_TO_BORDER,
        .label = "heightmap-sampler",
    });

    heightmap.Load("assets/heightmaps/heightmap.png");
    plane = batteries::CreatePlane(50.0f, 50.0f, 100);
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
    const Terrain::vs_params_t vs_terrain_params = {
        .view_proj = view_proj,
        .model = plane.transform.matrix(),
        .camera_pos = camera.position,
        .scale = debug.scale,
    };

    sg_begin_pass(&framebuffer.pass);
    sg_apply_pipeline(terrain.pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_terrain_params));
    sg_apply_bindings({
        .vertex_buffers[0] = plane.mesh.vertex_buffer,
        .index_buffer = plane.mesh.index_buffer,
        .vs = {
            .images[0] = heightmap.image,
            .samplers[0] = sampler,
        },
        .fs = {
            .images[0] = heightmap.image,
            .samplers[0] = sampler,
        },
    });
    sg_draw(0, plane.mesh.indices.size(), 1);
    sg_end_pass();

    // render framebuffer
    sg_begin_pass(&pass);
    sg_apply_pipeline(framebuffer.pipeline);
    sg_apply_bindings(&framebuffer.bindings);
    sg_draw(0, 6, 1);
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);
    if (ImGui::CollapsingHeader("Landmass"))
    {
        ImGui::SliderFloat("Scale", &debug.scale, 1.0f, 100.0f);
    }
    ImGui::End();
}