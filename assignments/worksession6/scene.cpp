#include "scene.h"
#include "imgui/imgui.h"

static int island_palette_index = 0;
static std::vector<std::tuple<std::string, nintendo::Palette>> island_palette{
    {"Sunny Day", {.highlight = {1.00f, 1.00f, 1.00f}, .shadow = {0.60f, 0.54f, 0.52f}}},
    {"Bright Night", {.highlight = {0.47f, 0.58f, 0.68f}, .shadow = {0.32f, 0.39f, 0.57f}}},
    {"Rainy Day", {.highlight = {0.62f, 0.69f, 0.67f}, .shadow = {0.50f, 0.55f, 0.50f}}},
    {"Rainy Night", {.highlight = {0.24f, 0.36f, 0.54f}, .shadow = {0.25f, 0.31f, 0.31f}}},
};

static int ocean_palette_index = 0;
static std::vector<std::tuple<std::string, nintendo::Palette>> ocean_palette{
    {"Sunny Day", {.highlight = {1.00f, 1.00f, 1.00f}, .shadow = {0.60f, 0.54f, 0.52f}}},
    {"Bright Night", {.highlight = {0.47f, 0.58f, 0.68f}, .shadow = {0.32f, 0.39f, 0.57f}}},
    {"Rainy Day", {.highlight = {0.62f, 0.69f, 0.67f}, .shadow = {0.50f, 0.55f, 0.50f}}},
    {"Rainy Night", {.highlight = {0.24f, 0.36f, 0.54f}, .shadow = {0.25f, 0.31f, 0.31f}}},
};

static int window_palette_index = 0;
static std::vector<std::tuple<std::string, nintendo::Palette>> window_palette{
    {"Day", {.highlight = {0.00f, 0.00f, 0.00f}, .shadow = {0.00f, 0.00f, 0.00f}}},
    {"Night", {.highlight = {1.00f, 1.00f, 0.00f}, .shadow = {0.00f, 0.00f, 0.00f}}},
};

Scene::Scene()
{
    camera.position = {115.0f, 75.0f, 850.0f};

    island.Load("/assets/windwaker/island/island.obj");
    sea.Load("/assets/windwaker/island/sea.obj");
    lights.Load("/assets/windwaker/island/window_lights.obj");

    island.transform.scale = {0.05f, 0.05f, 0.05f};
    sea.transform.scale = {0.05f, 0.05f, 0.05f};
    lights.transform.scale = {0.05f, 0.05f, 0.05f};
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
    const GeometryPass::vs_params_t vs_geometry_params = {
        .view_proj = view_proj,
        .model = island.transform.matrix(),
    };
    const GeometryPass::fs_params_t fs_geometry_params = {
        .palette = std::get<nintendo::Palette>(island_palette[island_palette_index]),
    };
    const WaterPass::vs_params_t vs_water_params = {
        .view_proj = view_proj,
        .model = sea.transform.matrix(),
    };
    const WaterPass::fs_params_t fs_water_params = {
        .palette = std::get<nintendo::Palette>(window_palette[window_palette_index]),
    };
    const LightsPass::vs_params_t vs_lights_params = {
        .view_proj = view_proj,
        .model = lights.transform.matrix(),
    };
    const LightsPass::fs_params_t fs_lights_params = {
        .palette = std::get<nintendo::Palette>(window_palette[1]),
    };

    sg_begin_pass(&framebuffer.pass);
    if (island.loaded && sea.loaded && lights.loaded)
    {
        // render island
        for (auto i = 0; i < island.mesh.groups.size(); i++)
        {
            auto group = island.mesh.groups[i];
            // apply windwaker island pipeline and uniforms
            sg_apply_pipeline(geometrypass.pipeline);
            sg_apply_uniforms(0, SG_RANGE(vs_geometry_params));
            sg_apply_uniforms(1, SG_RANGE(fs_geometry_params));
            sg_apply_bindings({
                .vertex_buffers[0] = island.mesh.vertex_buffer,
                .index_buffer = island.mesh.index_buffer,
                .images[0] = island.textures[i].image,
                .samplers[0] = island.mesh.sampler,
            });
            sg_draw(group.index_offset, group.face_count * 3, 1);
        }

        // render sea
        for (auto i = 0; i < sea.mesh.groups.size(); i++)
        {
            auto group = sea.mesh.groups[i];
            // apply windwaker island pipeline and uniforms
            sg_apply_pipeline(waterpass.pipeline);
            sg_apply_uniforms(0, SG_RANGE(vs_water_params));
            sg_apply_uniforms(1, SG_RANGE(fs_water_params));
            sg_apply_bindings({
                .vertex_buffers[0] = sea.mesh.vertex_buffer,
                .index_buffer = sea.mesh.index_buffer,
                .images[0] = sea.textures[i].image,
                .samplers[0] = sea.mesh.sampler,
            });
            sg_draw(group.index_offset, group.face_count * 3, 1);
        }

        // render window lights
        for (auto i = 0; i < lights.mesh.groups.size(); i++)
        {
            auto group = lights.mesh.groups[i];
            // apply windwaker island pipeline and uniforms
            sg_apply_pipeline(lightspass.pipeline);
            sg_apply_uniforms(0, SG_RANGE(vs_lights_params));
            sg_apply_uniforms(1, SG_RANGE(fs_lights_params));
            sg_apply_bindings({
                .vertex_buffers[0] = lights.mesh.vertex_buffer,
                .index_buffer = lights.mesh.index_buffer,
                .images[0] = lights.textures[i].image,
                .samplers[0] = lights.mesh.sampler,
            });
            sg_draw(group.index_offset, group.face_count * 3, 1);
        }
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
    cameracontroller.Debug();
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    if (ImGui::CollapsingHeader("Camera"))
    {
        ImGui::Text("Position: %.2f, %.2f, %.2f", camera.position[0], camera.position[1], camera.position[2]);
    }

    ImGui::Text("Presets");
    if (ImGui::BeginCombo("Island Palette", std::get<std::string>(island_palette[island_palette_index]).c_str()))
    {
        for (auto n = 0; n < island_palette.size(); ++n)
        {
            auto is_selected = (std::get<0>(island_palette[island_palette_index]) == std::get<0>(island_palette[n]));
            if (ImGui::Selectable(std::get<std::string>(island_palette[n]).c_str(), is_selected))
            {
                island_palette_index = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Ocean Palette", std::get<std::string>(ocean_palette[ocean_palette_index]).c_str()))
    {
        for (auto n = 0; n < ocean_palette.size(); ++n)
        {
            auto is_selected = (std::get<0>(ocean_palette[ocean_palette_index]) == std::get<0>(ocean_palette[n]));
            if (ImGui::Selectable(std::get<std::string>(ocean_palette[n]).c_str(), is_selected))
            {
                ocean_palette_index = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Window Palette", std::get<std::string>(window_palette[window_palette_index]).c_str()))
    {
        for (auto n = 0; n < window_palette.size(); ++n)
        {
            auto is_selected = (std::get<0>(window_palette[window_palette_index]) == std::get<0>(window_palette[n]));
            if (ImGui::Selectable(std::get<std::string>(window_palette[n]).c_str(), is_selected))
            {
                window_palette_index = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::End();
}