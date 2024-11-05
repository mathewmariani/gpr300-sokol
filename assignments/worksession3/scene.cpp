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
    float lod_bias = 14.5f;
    float tiling = 100.0f;
    float time;
    float top_scale = 1.00f;
    float bottom_scale = 1.00f;
    float brightness_lower_cutoff = 0.40f;
    float brightness_upper_cutoff = 0.93f;
} debug;

Scene::Scene()
{
    pass.action.colors[0].clear_value = {0.0f, 0.4549f, 0.8980f, 1.0f};

    auto init_water_texture = [this]()
    {
        mipmap_img = sg_alloc_image();
        batteries::load_mipmap({
            .img_id = mipmap_img,
            .path = {
                .mip0 = "assets/sunshine/water128.png",
                .mip1 = "assets/sunshine/water64.png",
                .mip2 = "assets/sunshine/water32.png",
                .mip3 = "assets/sunshine/water16.png",
                .mip4 = "assets/sunshine/water8.png",
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
    plane = batteries::CreatePlane(400.0f, 400.0f, 1);

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
    };
    const Water::fs_params_t fs_water_params = {
        .lod_bias = debug.lod_bias,
        .tiling = debug.tiling,
        .time = (float)time.absolute,
        .top_scale = debug.top_scale,
        .bottom_scale = debug.bottom_scale,
        .brightness_lower_cutoff = debug.brightness_lower_cutoff,
        .brightness_upper_cutoff = debug.brightness_upper_cutoff,
    };

    sg_begin_pass(&framebuffer.pass);
    sg_apply_pipeline(water.pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_water_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_water_params));
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
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);

    if (ImGui::CollapsingHeader("Camera"))
    {
        ImGui::Text("Position: %.2f, %.2f, %.2f", camera.position[0], camera.position[1], camera.position[2]);
    }

    ImGui::SliderFloat("lod_bias", &debug.lod_bias, 0.0f, 30.0f, "%.2f");
    ImGui::SliderFloat("brightness_lower_cutoff", &debug.brightness_lower_cutoff, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("brightness_upper_cutoff", &debug.brightness_upper_cutoff, 0.0f, 1.0f, "%.2f");

    if (ImGui::CollapsingHeader("Texture"))
    {
        ImGui::SliderFloat("debug.tiling", &debug.tiling, 1.0f, 1000.0f);
        ImGui::SliderFloat("debug.top_scale", &debug.top_scale, 0.0f, 1.0f);
        ImGui::SliderFloat("debug.bottom_scale", &debug.bottom_scale, 0.0f, 1.0f);
    }

    ImGui::End();
}