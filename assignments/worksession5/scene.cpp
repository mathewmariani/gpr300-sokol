#include "scene.h"
#include "batteries/assets.h"
#include "imgui/imgui.h"

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};
static uint8_t file_buffer[1024 * 1024 * 5];

static sg_image gradients[3];

Scene::Scene()
{
    auto load_gradients = [this]()
    {
#define LOAD_GRADIENT(i, filepath)                 \
    gradients[i] = sg_alloc_image();               \
    batteries::load_img({.image_id = gradients[i], \
                         .path = filepath,         \
                         .buffer = SG_RANGE(file_buffer)})
        LOAD_GRADIENT(0, "assets/transitions/gradient1.png");
        LOAD_GRADIENT(1, "assets/transitions/gradient2.png");
        LOAD_GRADIENT(2, "assets/transitions/gradient3.png");
#undef LOAD_GRADIENT
    };

    auto load_suzanne = [this]()
    {
        suzanne.mesh.vbuf = sg_alloc_buffer();
        suzanne.mesh.bindings = (sg_bindings){
            .vertex_buffers[0] = suzanne.mesh.vbuf,
        };
        batteries::load_obj({
            .buffer_id = suzanne.mesh.vbuf,
            .mesh = &suzanne.mesh,
            .path = "assets/suzanne.obj",
            .buffer = SG_RANGE(file_buffer),
        });
    };

    ambient = (batteries::ambient_t){
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    light = (batteries::light_t){
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
    };

    material = (batteries::material_t){
        .ambient = {0.5f, 0.5f, 0.5f},
        .diffuse = {0.5f, 0.5f, 0.5f},
        .specular = {0.5f, 0.5f, 0.5f},
        .shininess = 128.0f,
    };

    load_gradients();
    load_suzanne();

    transition.bindings.fs = {
        .images[0] = gradients[0],
        .samplers[0] = sg_make_sampler({
            .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
            .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
        }),
    };
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

    // initialize uniform data
    const BlinnPhong::vs_params_t vs_blinnphong_params{
        .view_proj = view_proj,
        .model = suzanne.transform.matrix(),
    };
    const BlinnPhong::fs_params_t fs_blinnphong_params{
        .material = material,
        .light = light,
        .ambient = ambient,
        .camera_position = camera.position,
    };

    blinnPhong.Apply(vs_blinnphong_params, fs_blinnphong_params);
    suzanne.Render();

    // apply post process
    transition.Render();
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);
    ImGui::DragFloat("Cutoff", &transition.settings.fs_params.cutoff, 0.01f, 0.00f, 1.00f);
    ImGui::ColorEdit3("Fadeout Color", &transition.settings.fs_params.color[0]);
    if (ImGui::Button("Gradient 1"))
    {
        transition.bindings.fs.images[0] = gradients[0];
    }
    if (ImGui::Button("Gradient 2"))
    {
        transition.bindings.fs.images[0] = gradients[1];
    }
    if (ImGui::Button("Gradient 3"))
    {
        transition.bindings.fs.images[0] = gradients[2];
    }
    ImGui::End();
}