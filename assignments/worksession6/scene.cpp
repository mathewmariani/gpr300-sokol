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

    island.Load("assets/windwaker/island/island.obj");
    sea.Load("assets/windwaker/island/sea.obj");
    lights.Load("assets/windwaker/island/window_lights.obj");

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
    const auto view_proj = camera.projection() * camera.view();

    // initialize uniform data
    const GeometryPass::vs_params_t vs_geometry_params = {
        .view_proj = view_proj,
        .model = island.transform.matrix(),
    };
    const GeometryPass::fs_params_t fs_geometry_params = {
        .palette = std::get<nintendo::Palette>(island_palette[island_palette_index]),
    };
    const SeaPass::vs_params_t vs_sea_params = {
        .view_proj = view_proj,
        .model = sea.transform.matrix(),
    };
    const SeaPass::fs_params_t fs_sea_params = {
        .palette = std::get<nintendo::Palette>(window_palette[window_palette_index]),
    };
    const WindowLightsPass::vs_params_t vs_windowlights_params = {
        .view_proj = view_proj,
        .model = lights.transform.matrix(),
    };
    const WindowLightsPass::fs_params_t fs_windowlights_params = {
        .palette = std::get<nintendo::Palette>(window_palette[1]),
    };

    sg_begin_pass(&framebuffer.pass);
    if (island.loaded && sea.loaded && lights.loaded)
    {
        // render island
        for (auto i = 0; i < island.mesh.groups.size(); i++)
        {
            auto group = island.mesh.groups[i];

            // update bindings
            sg_bindings bindings = (sg_bindings){
                .vertex_buffers[0] = island.mesh.vertex_buffer,
                .fs.images[0] = island.textures[i].image,
                .fs.samplers = island.mesh.sampler,
            };
            // apply windwaker island pipeline and uniforms
            sg_apply_pipeline(geometrypass.pipeline);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_geometry_params));
            sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_geometry_params));
            sg_apply_bindings(bindings);
            sg_draw(group.index_offset, group.face_count * 3, 1);
        }

        // render sea
        for (auto i = 0; i < sea.mesh.groups.size(); i++)
        {
            auto group = sea.mesh.groups[i];

            // update bindings
            sg_bindings bindings = (sg_bindings){
                .vertex_buffers[0] = sea.mesh.vertex_buffer,
                .fs.images[0] = sea.textures[i].image,
                .fs.samplers = sea.mesh.sampler,
            };
            // apply windwaker island pipeline and uniforms
            sg_apply_pipeline(seapass.pipeline);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_sea_params));
            sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_sea_params));
            sg_apply_bindings(bindings);
            sg_draw(group.index_offset, group.face_count * 3, 1);
        }

        // render window lights
        for (auto i = 0; i < lights.mesh.groups.size(); i++)
        {
            auto group = lights.mesh.groups[i];

            // update bindings
            sg_bindings bindings = (sg_bindings){
                .vertex_buffers[0] = lights.mesh.vertex_buffer,
                .fs.images[0] = lights.textures[i].image,
                .fs.samplers = lights.mesh.sampler,
            };
            // apply windwaker island pipeline and uniforms
            sg_apply_pipeline(windowlightspass.pipeline);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_windowlights_params));
            sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_windowlights_params));
            sg_apply_bindings(bindings);
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
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);

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