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
    auto init_water = [this]()
    {
        water_obj.img = sg_alloc_image();
        sg_sampler smp = sg_make_sampler({
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
            .wrap_u = SG_WRAP_REPEAT,
            .wrap_v = SG_WRAP_REPEAT,
            .label = "water-sampler",
        });

        // generate shape geometries
        sshape_vertex_t vertices[2 * 1024];
        uint16_t indices[4 * 1024];
        sshape_buffer_t buf = {
            .vertices.buffer = SSHAPE_RANGE(vertices),
            .indices.buffer = SSHAPE_RANGE(indices),
        };
        sshape_plane_t plane = {
            .width = 100.0f,
            .depth = 100.0f,
            .tiles = 10,
        };
        buf = sshape_build_plane(&buf, &plane);
        water_obj.plane.draw = sshape_element_range(&buf);

        water_obj.plane.transform.position = glm::vec3(0.0f, -1.0f, 0.0f);

        // one vertex/index-buffer-pair for all shapes
        const auto vbuf_desc = sshape_vertex_buffer_desc(&buf);
        const auto ibuf_desc = sshape_index_buffer_desc(&buf);

        water_obj.bind = (sg_bindings){
            .vertex_buffers[0] = sg_make_buffer(&vbuf_desc),
            .index_buffer = sg_make_buffer(&ibuf_desc),
            .fs = {
                .images[0] = water_obj.img,
                .samplers[0] = smp,
            },
        };

        batteries::load_img({
            .image_id = water_obj.img,
            .path = "assets/materials/water.png",
            .buffer = SG_RANGE(file_buffer),
        });
    };

    ocean = {
        .color = glm::vec3(0.00f, 0.31f, 0.85f),
        .direction = glm::vec3(0.5f),
        .scale = 10.0f,
        .strength = 1.0f,
        .tiling = 10.0f,
        .top_scale = 0.90f,
        .bottom_scale = 0.02f,
    },

    init_water();
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
        .scale = ocean.scale,
        .strength = ocean.strength,
        .time = (float)time.absolute,
        .color = ocean.color,
        .direction = ocean.direction,
        .tiling = ocean.tiling,
        .top_scale = ocean.top_scale,
        .bottom_scale = ocean.bottom_scale,
    };
    const Water::fs_params_t fs_water_params = {
        .scale = ocean.scale,
        .strength = ocean.strength,
        .time = (float)time.absolute,
        .color = ocean.color,
        .direction = ocean.direction,
        .tiling = ocean.tiling,
        .top_scale = ocean.top_scale,
        .bottom_scale = ocean.bottom_scale,
    };

    water.Apply(vs_water_params, fs_water_params);
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);

    if (ImGui::CollapsingHeader("Color"))
    {
        ImGui::ColorEdit3("Color", &ocean.color[0]);
        ImGui::SliderFloat("Upper Blend", &ocean.top_scale, 0.0f, 1.0f);
        ImGui::SliderFloat("Bottom Blend", &ocean.bottom_scale, 0.0f, 1.0f);
    }
    if (ImGui::CollapsingHeader("Texture"))
    {
        ImGui::SliderFloat("Tilling", &ocean.tiling, 1.0f, 10.0f);
        ImGui::SliderFloat2("Scroll Direction", &ocean.direction[0], -1.0f, 1.0f);
    }
    if (ImGui::CollapsingHeader("Wave"))
    {
        ImGui::SliderFloat("Scale", &ocean.scale, 1.0f, 100.0f);
        ImGui::SliderFloat("Strength", &ocean.strength, 1.0f, 100.0f);
    }
    ImGui::End();
}