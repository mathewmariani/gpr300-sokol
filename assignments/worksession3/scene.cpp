#include "scene.h"

#include "imgui/imgui.h"

#include "batteries/texture.h"

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};

static const uint32_t mip_colors[9] = {
    0xFF0000FF, // red
    0xFF00FF00, // green
    0xFFFF0000, // blue
    0xFFFF00FF, // magenta
    0xFFFFFF00, // cyan
    0xFF00FFFF, // yellow
    0xFFFF00A0, // violet
    0xFFFFA0FF, // orange
    0xFFA000FF, // purple
};

static int sampler_index = 0;

static int num_mips = 5;
sg_image mipmap_color_img;
sg_sampler smp;
struct
{
    uint32_t mip0[16384]; // 128x128
    uint32_t mip1[4096];  // 64*64
    uint32_t mip2[1024];  // 32*32
    uint32_t mip3[256];   // 16*16
    uint32_t mip4[64];    // 8*8
} pixels;

static uint8_t cubemap_buffer[1024 * 1024 * 10];
sg_image mipmap_img;

static struct
{
    bool mipmap_colors = false;
    float lod_bias = 14.5f;
    float tiling = 100.0f;
    float time;
    float top_scale = 1.00f;
    float bottom_scale = 1.00f;
} debug;

Scene::Scene()
{
    pass.action.colors[0].clear_value = {0.0f, 0.4549f, 0.8980f, 1.0f};
    // framebuffer.pass.action.colors[0].clear_value = {0.0f, 0.4549f, 0.8980f, 1.0f};

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

    auto init_water_texture = [this]()
    {
        // create image with mipmap content, different colors and checkboard pattern
        sg_image_data img_data;
        uint32_t *ptr = pixels.mip0;
        for (int mip_index = 1; mip_index <= 5; mip_index++)
        {
            const int dim = 1 << (8 - mip_index);
            img_data.subimage[0][mip_index - 1].ptr = ptr;
            img_data.subimage[0][mip_index - 1].size = (size_t)(dim * dim * 4);
            for (int y = 0; y < dim; y++)
            {
                for (int x = 0; x < dim; x++)
                {
                    *ptr++ = mip_colors[mip_index - 1];
                }
            }
        }
        mipmap_color_img = sg_make_image((sg_image_desc){
            .width = 128,
            .height = 128,
            .num_mipmaps = num_mips,
            .pixel_format = SG_PIXELFORMAT_RGBA8,
            .data = img_data,
        });
        smp = sg_make_sampler((sg_sampler_desc){
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
            .mipmap_filter = SG_FILTER_LINEAR,
            .wrap_u = SG_WRAP_REPEAT,
            .wrap_v = SG_WRAP_REPEAT,
        });
    };

    water_obj.plane = batteries::BuildPlane();
    init_water_texture();
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

    // initialize uniform data
    const Water::vs_params_t vs_water_params = {
        .view_proj = view_proj,
        .model = water_obj.plane.transform.matrix(),
        .camera_pos = camera.position,
    };
    const Water::fs_params_t fs_water_params = {
        .lod_bias = debug.lod_bias,
        .tiling = debug.tiling,
        .time = (float)time.absolute,
        .top_scale = debug.top_scale,
        .bottom_scale = debug.bottom_scale,
    };

    sg_image img = debug.mipmap_colors ? mipmap_color_img : mipmap_img;

    sg_begin_pass(&framebuffer.pass);
    sg_apply_pipeline(water.pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_water_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_water_params));
    // create bindings
    auto bindings = (sg_bindings){
        .vertex_buffers[0] = water_obj.plane.vertex_buffer,
        .index_buffer = water_obj.plane.index_buffer,
        .fs = {
            .images[0] = img,
            .samplers[0] = smp,
        },
    };
    sg_apply_bindings(&bindings);
    sg_draw(water_obj.plane.draw.base_element, water_obj.plane.draw.num_elements, 1);
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

    if (ImGui::CollapsingHeader("Camera"))
    {
        ImGui::Text("Position: %.2f, %.2f, %.2f", camera.position[0], camera.position[1], camera.position[2]);
    }

    ImGui::SliderFloat("lod_bias", &debug.lod_bias, 0.0f, +30.0f, "%.2f");

    if (ImGui::Button("mipmap_colors"))
    {
        debug.mipmap_colors = !debug.mipmap_colors;
    }

    if (ImGui::CollapsingHeader("Texture"))
    {
        ImGui::SliderFloat("debug.tiling", &debug.tiling, 1.0f, 1000.0f);
        ImGui::SliderFloat("debug.top_scale", &debug.top_scale, 0.0f, 1.0f);
        ImGui::SliderFloat("debug.bottom_scale", &debug.bottom_scale, 0.0f, 1.0f);
    }

    ImGui::End();
}