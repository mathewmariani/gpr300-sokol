#include "scene.h"

// batteries
#include "batteries/assets.h"

// imgui
#include "imgui/imgui.h"

#include <string>
#include <vector>

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};

enum {
    NO_POST,
    GRAYSCALE,
    BLUR,
    EDGEDETECTION,
    INVERSE,
    CHROMATIC_ABERRATION,
    CRT,
};

static struct {
    int index = 0;

    struct {
        float strength = 16.0f;
    } blue;

    struct {
        glm::vec3 offsets{0.009f, 0.006f, -0.006f};
    } chromatic_aberration;
} effect;

static std::vector<std::string> post_processing_effects = {
    "None",
    "Grayscale",
    "Kernel Blur",
    "Edge Detection",
    "Inverse",
    "Chromatic Aberration",
    "CRT",
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

    suzanne.Load("assets/suzanne.obj");

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

    // initialize uniform data
    const BlinnPhong::vs_params_t vs_blinnphong_params = {
        .view_proj = view_proj,
        .model = suzanne.transform.matrix(),
    };
    const BlinnPhong::fs_params_t fs_blinnphong_params = {
        .material = material,
        .light = light,
        .ambient = ambient,
        .camera_position = camera.position,
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
    sg_apply_pipeline(blinnphong.pipeline);
    sg_apply_uniforms(0, SG_RANGE(vs_blinnphong_params));
    sg_apply_uniforms(1, SG_RANGE(fs_blinnphong_params));

    // render suzanne
    if (suzanne.loaded)
    {
        sg_apply_bindings({
            .vertex_buffers[0] = suzanne.mesh.vertex_buffer,
            .index_buffer = suzanne.mesh.index_buffer,
        });
        sg_draw(0, suzanne.mesh.indices.size(), 1);
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

    // apply a post processing effect
    switch (effect.index)
    {
    case GRAYSCALE:
        sg_apply_pipeline(grayscaleRenderer.pipeline);
        break;
    case BLUR:
        sg_apply_pipeline(blurRenderer.pipeline);
        break;
    case EDGEDETECTION:
        sg_apply_pipeline(edgeDetectionRenderer.pipeline);
        break;
    case INVERSE:
        sg_apply_pipeline(inverseRenderer.pipeline);
        break;
    case CHROMATIC_ABERRATION:
        sg_apply_pipeline(chromaticAberrationRenderer.pipeline);
        break;
    case CRT:
        sg_apply_pipeline(crtRenderer.pipeline);
        break;
    case NO_POST:
    default:
        sg_apply_pipeline(framebuffer.pipeline);
        break;
    }
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

    if (ImGui::CollapsingHeader("Ambient"))
    {
        ImGui::SliderFloat("Intensity", &ambient.intensity, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color", &ambient.color[0]);
    }
    if (ImGui::CollapsingHeader("Light"))
    {
        ImGui::SliderFloat("Brightness", &light.brightness, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color", &light.color[0]);
    }
    if (ImGui::BeginCombo("Effect", post_processing_effects[effect.index].c_str()))
    {
        for (auto n = 0; n < post_processing_effects.size(); ++n)
        {
            auto is_selected = (post_processing_effects[effect.index] == post_processing_effects[n]);
            if (ImGui::Selectable(post_processing_effects[n].c_str(), is_selected))
            {
                effect.index = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    switch (effect.index)
    {
    case GRAYSCALE:
    case BLUR:
    case EDGEDETECTION:
    case INVERSE:
    case CHROMATIC_ABERRATION:
    case CRT:
    case NO_POST:
    default:
        break;
    }
    ImGui::End();
}