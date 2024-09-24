#include "scene.h"

#include "batteries/assets.h"

#include <unordered_map>

static glm::vec4 light_orbit_radius = {2.0f, 2.0f, -2.0f, 1.0f};

static uint8_t file_buffer[1024 * 1024 * 5];

Scene::Scene()
{
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
    },

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

    ambient.direction = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - light.position);
}

void Scene::Render(void)
{
    const auto view_proj = camera.projection() * camera.view();

    // depth pass matrices
    const auto light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
    const auto light_view = glm::lookAt(light.position, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    const auto light_view_proj = light_proj * light_view;

    // initialize uniform data
    const Depth::vs_params_t vs_depth_params = {
        .view_proj = view_proj,
        .model = suzanne.transform.matrix(),
    };
    const Shadow::vs_params_t vs_shadow_params = {
        .view_proj = view_proj,
        .model = suzanne.transform.matrix(),
        .light_view_proj = light_view_proj,
    };
    const Shadow::fs_params_t fs_shadow_params = {
        .material = material,
        .light = light,
        .ambient = ambient,
        .camera_position = camera.position,
    };
    const batteries::Gizmo::vs_params_t vs_gizmo_params = {
        .view_proj = view_proj,
        .model = glm::translate(glm::mat4(1.0f), light.position),
    };
    const batteries::Gizmo::fs_params_t fs_gizmo_params = {
        .color = light.color,
    };
    const batteries::Skybox::vs_params_t vs_skybox_params = {
        .view_proj = camera.projection() * glm::mat4(glm::mat3(camera.view())),
    };

    sg_begin_pass(&depthbuffer.pass);
    depth.Render(vs_depth_params, suzanne);
    sg_end_pass();

    sg_begin_pass({.action = pass_action, .attachments = framebuffer.attachments});
    shadow.Render(vs_shadow_params, fs_shadow_params, depthbuffer.depth, suzanne);
    gizmo.Render(vs_gizmo_params, fs_gizmo_params);
    skybox.Render(vs_skybox_params);
    sg_end_pass();

    framebuffer.Render();

    // sg_end_pass();
    // sg_commit();
}