#include "scene.h"

// batteries
#include "batteries/assets.h"

// imgui
#include "imgui/imgui.h"

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};

static int model_index = 0;
static std::vector<std::string> model_paths{
    "assets/smashbros/togezoshell/togezoshell.obj",
    "assets/smashbros/greenshell/greenshell.obj",
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

    sphere = batteries::CreateSphere(1.0f, 4);
    sphere.transform.scale = {0.25f, 0.25f, 0.25f};

    auto size = model_paths.size();
    models.resize(size);
    for (auto i = 0; i < size; ++i)
    {
        models[i].Load(model_paths[i]);
        models[i].transform.scale = glm::vec3(0.5f);
    }
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    static auto ry = 0.0f;
    ry += dt;

    // sugar: rotate light
    const auto rym = glm::rotate(ry, glm::vec3(0.0f, 1.0f, 0.0f));
    light.position = rym * light_orbit_radius;
}

void Scene::Render(void)
{
    const auto view_proj = camera.projection() * camera.view();
    auto model = models[model_index];

    // initialize uniform data
    const PhysicallyBasedRendering::vs_params_t vs_pbr_params = {
        .view_proj = view_proj,
        .model = model.transform.matrix(),
    };
    const PhysicallyBasedRendering::fs_params_t fs_pbr_params = {
        .light = light,
        .camera_position = camera.position,
    };
    const batteries::Gizmo::vs_params_t vs_gizmo_params = {
        .view_proj = view_proj,
        .model = glm::translate(sphere.transform.matrix(), light.position),
    };
    const batteries::Gizmo::fs_params_t fs_gizmo_params = {
        .color = light.color,
    };
    const batteries::Skybox::vs_params_t vs_skybox_params = {
        .view_proj = camera.projection() * glm::mat4(glm::mat3(camera.view())),
    };

    sg_begin_pass(&framebuffer.pass);
    // apply pbr pipeline and uniforms
    sg_apply_pipeline(pbr.pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_pbr_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_pbr_params));
    // render suzanne
    if (model.loaded)
    {
        // create bindings
        auto bindings = (sg_bindings){
            .vertex_buffers[0] = model.mesh.vertex_buffer,
            // .index_buffer = suzanne.mesh.index_buffer,
            .fs = {
                .images = {
                    [0] = model.col.image,
                    [1] = model.mtl.image,
                    [2] = model.rgh.image,
                    [3] = model.ao.image,
                    [4] = model.spc.image,
                },
                .samplers[0] = model.mesh.sampler,
            },
        };
        sg_apply_bindings(bindings);
        sg_draw(0, model.mesh.num_faces * 3, 1);
    }

    // render light sources
    sg_apply_pipeline(gizmo.pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_gizmo_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_gizmo_params));
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
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);

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