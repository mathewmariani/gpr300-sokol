#include "scene.h"

// imgui
#include "imgui/imgui.h"

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};

static int palette_index = 0;
static std::vector<std::tuple<std::string, ToonShading::Palette>> palette{
    {"Sunny Day", {.highlight = {1.00f, 1.00f, 1.00f}, .shadow = {0.60f, 0.54f, 0.52f}}},
    {"Bright Night", {.highlight = {0.47f, 0.58f, 0.68f}, .shadow = {0.32f, 0.39f, 0.57f}}},
    {"Rainy Day", {.highlight = {0.62f, 0.69f, 0.67f}, .shadow = {0.50f, 0.55f, 0.50f}}},
    {"Rainy Night", {.highlight = {0.24f, 0.36f, 0.54f}, .shadow = {0.25f, 0.31f, 0.31f}}},
};

static int model_index = 0;
static std::vector<std::string> model_paths{
    "./assets/windwaker/skull/skull.obj",
    "./assets/windwaker/pot_water/pot_water.obj",
};

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

    auto size = model_paths.size();
    models.resize(size);
    for (auto i = 0; i < size; ++i)
    {
        models[i].Load(model_paths[i]);
        models[i].transform.scale = glm::vec3(0.05f);
    }

    zatoon.Load("./assets/windwaker/ZAtoon.png");

    sphere = batteries::CreateSphere(1.0f, 4);
    sphere.transform.scale = {0.25f, 0.25f, 0.25f};
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
    auto model = models[model_index];

    // initialize uniform data
    const ToonShading::vs_params_t vs_toon_params = {
        .view_proj = view_proj,
        .model = model.transform.matrix(),
    };
    const ToonShading::fs_params_t fs_toon_params = {
        .light = light,
        .palette = std::get<ToonShading::Palette>(palette[palette_index]),
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
    sg_apply_pipeline(toonshading.pipeline);
    sg_apply_uniforms(0, SG_RANGE(vs_toon_params));
    sg_apply_uniforms(1, SG_RANGE(fs_toon_params));
    // render suzanne
    if (model.loaded)
    {
        sg_apply_bindings({
            .vertex_buffers[0] = model.mesh.vertex_buffer,
            .index_buffer = model.mesh.index_buffer,
            .images = {
                [0] = model.albedo.image,
                [1] = zatoon.image,
            },
            .samplers[0] = model.mesh.sampler,
        });
        sg_draw(0, model.mesh.num_faces * 3, 1);
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
    sg_apply_pipeline(framebuffer.pipeline);
    sg_apply_bindings(&framebuffer.bindings);
    sg_draw(0, 6, 1);
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    ImGui::Text("Presets");
    if (ImGui::BeginCombo("Palette", std::get<std::string>(palette[palette_index]).c_str()))
    {
        for (auto n = 0; n < palette.size(); ++n)
        {
            auto is_selected = (std::get<0>(palette[palette_index]) == std::get<0>(palette[n]));
            if (ImGui::Selectable(std::get<std::string>(palette[n]).c_str(), is_selected))
            {
                palette_index = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::ColorEdit3("Highlight", &std::get<ToonShading::Palette>(palette[palette_index]).highlight[0]);
    ImGui::ColorEdit3("Shadow", &std::get<ToonShading::Palette>(palette[palette_index]).shadow[0]);

    ImGui::Text("Model");
    if (ImGui::BeginCombo("Model", model_paths[model_index].c_str()))
    {
        for (auto n = 0; n < model_paths.size(); ++n)
        {
            auto is_selected = (model_paths[model_index] == model_paths[n]);
            if (ImGui::Selectable(model_paths[n].c_str(), is_selected))
            {
                model_index = n;
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