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
static float lod_bias = 14.5f;
static int num_mips = 5;
sg_image img;
sg_sampler smp;
struct
{
    uint32_t mip0[4096]; // 64*64
    uint32_t mip1[1024]; // 32*32
    uint32_t mip2[256];  // 16*16
    uint32_t mip3[64];   // 8*8
    uint32_t mip4[16];   // 4*4
} pixels;

struct
{
    batteries::Texture water128;
    batteries::Texture water64;
    batteries::Texture water32;
    batteries::Texture water16;
    batteries::Texture water8;
} texture;

Scene::Scene()
{
    texture.water128.Load("assets/sunshine/water128.png");
    texture.water64.Load("assets/sunshine/water64.png");
    texture.water32.Load("assets/sunshine/water32.png");
    texture.water16.Load("assets/sunshine/water16.png");
    texture.water8.Load("assets/sunshine/water8.png");

    auto init_water_texture = [this]()
    {
        // create image with mipmap content, different colors and checkboard pattern
        sg_image_data img_data;
        uint32_t *ptr = pixels.mip0;
        for (int mip_index = 2; mip_index <= 6; mip_index++)
        {
            const int dim = 1 << (8 - mip_index);
            img_data.subimage[0][mip_index - 2].ptr = ptr;
            img_data.subimage[0][mip_index - 2].size = (size_t)(dim * dim * 4);
            for (int y = 0; y < dim; y++)
            {
                for (int x = 0; x < dim; x++)
                {
                    *ptr++ = mip_colors[mip_index - 2];
                }
            }
        }
        img = sg_make_image((sg_image_desc){
            .width = 64,
            .height = 64,
            .num_mipmaps = num_mips,
            .pixel_format = SG_PIXELFORMAT_RGBA8,
            .data = img_data,
        });

        smp = sg_make_sampler((sg_sampler_desc){
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
            .mipmap_filter = SG_FILTER_LINEAR,
            .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
            .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
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
        .lod_bias = lod_bias,
    };

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

    ImGui::SliderFloat("lod_bias", &lod_bias, 0.0f, +30.0f, "%.2f");

    ImGui::End();
}