#include "scene.h"

// batteries
#include "batteries/assets.h"

// imgui
#include "imgui/imgui.h"

static uint8_t cubemap_buffer[1024 * 1024 * 10];
sg_image mipmap_img;
sg_sampler mimap_smp;

static struct
{
    glm::vec3 color = {0.00f, 0.31f, 0.85f};
    glm::vec2 direction = {0.5f, 0.5f};
    float scale = 10.0f;
    float strength = 1.0f;
    float tiling = 10.0f;
    float top_scale = 0.90f;
    float bottom_scale = 0.02f;
    float lod_bias = 14.5f;
} debug;

Scene::Scene()
{
    auto init_water_texture = [this]()
    {
        mipmap_img = sg_alloc_image();
        batteries::load_mipmap({
            .img_id = mipmap_img,
            .path = {
                .mip0 = "assets/windwaker/water128.png",
                .mip1 = "assets/windwaker/water64.png",
                .mip2 = "assets/windwaker/water32.png",
                .mip3 = "assets/windwaker/water16.png",
                .mip4 = "assets/windwaker/water8.png",
            },
            .buffer_ptr = cubemap_buffer,
            .buffer_offset = 1024 * 1024,
        });
        mimap_smp = sg_make_sampler({
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
            .mipmap_filter = SG_FILTER_LINEAR,
            .wrap_u = SG_WRAP_REPEAT,
            .wrap_v = SG_WRAP_REPEAT,
        });
    };

    init_water_texture();
    plane = batteries::CreatePlane(400.0f, 400.0f, 10);

    cameracontroller.Configure({
        .mode = (int)batteries::CameraController::Mode::Orbit,
        .pitch = 30.0f,
        .yaw = 0.0f,
        .distance = 10.0f,
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
    const auto view_proj = camera.Projection() * camera.View();

    // initialize uniform data
    const Water::vs_params_t vs_water_params = {
        .view_proj = view_proj,
        .model = plane.transform.matrix(),
        .camera_pos = camera.position,
        .scale = debug.scale,
        .strength = debug.strength,
        .time = (float)time.absolute,
        .color = debug.color,
        .direction = debug.direction,
        .tiling = debug.tiling,
        .top_scale = debug.top_scale,
        .bottom_scale = debug.bottom_scale,
        .lod_bias = debug.lod_bias,
    };
    const Water::fs_params_t fs_water_params = {
        .scale = debug.scale,
        .strength = debug.strength,
        .time = (float)time.absolute,
        .color = debug.color,
        .direction = debug.direction,
        .tiling = debug.tiling,
        .top_scale = debug.top_scale,
        .bottom_scale = debug.bottom_scale,
        .lod_bias = debug.lod_bias,
    };

    sg_begin_pass(&framebuffer.pass);
    sg_apply_pipeline(water.pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_water_params));
    sg_apply_bindings({
        .vertex_buffers[0] = plane.mesh.vertex_buffer,
        .index_buffer = plane.mesh.index_buffer,
        .fs = {
            .images[0] = mipmap_img,
            .samplers[0] = mimap_smp,
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

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    if (ImGui::CollapsingHeader("Camera"))
    {
        ImGui::Text("Position: %.2f, %.2f, %.2f", camera.position[0], camera.position[1], camera.position[2]);
    }

    ImGui::SliderFloat("lod_bias", &debug.lod_bias, 0.0f, 30.0f, "%.2f");

    if (ImGui::CollapsingHeader("Color"))
    {
        ImGui::ColorEdit3("Color", &debug.color[0]);
        ImGui::SliderFloat("Upper Blend", &debug.top_scale, 0.0f, 1.0f);
        ImGui::SliderFloat("Bottom Blend", &debug.bottom_scale, 0.0f, 1.0f);
    }
    if (ImGui::CollapsingHeader("Texture"))
    {
        ImGui::SliderFloat("Tilling", &debug.tiling, 1.0f, 10.0f);
        ImGui::SliderFloat2("Scroll Direction", &debug.direction[0], -1.0f, 1.0f);
    }
    if (ImGui::CollapsingHeader("Wave"))
    {
        ImGui::SliderFloat("Scale", &debug.scale, 1.0f, 100.0f);
        ImGui::SliderFloat("Strength", &debug.strength, 1.0f, 100.0f);
    }
    ImGui::End();
}