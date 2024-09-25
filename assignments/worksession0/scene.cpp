#include "scene.h"

#include "batteries/assets.h"

#include <unordered_map>

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};
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

    togezoshell.mesh.vbuf = sg_alloc_buffer();
    batteries::load_obj({
        .buffer_id = togezoshell.mesh.vbuf,
        .mesh = &togezoshell.mesh,
        .path = "assets/objects/togezoshell/togezoshell.obj",
        .buffer = SG_RANGE(file_buffer),
    });

    // transform
    togezoshell.transform.rotation = glm::rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    togezoshell.transform.scale = glm::vec3(0.5f);

    material = {
        .col = sg_alloc_image(),
        .mtl = sg_alloc_image(),
        .rgh = sg_alloc_image(),
        .ao = sg_alloc_image(),
        .spc = sg_alloc_image(),
    };

    togezoshell.mesh.bindings = (sg_bindings){
        .vertex_buffers[0] = togezoshell.mesh.vbuf,
        .fs = {
            .images = {
                [0] = material.col,
                [1] = material.mtl,
                [2] = material.rgh,
                [3] = material.ao,
                [4] = material.spc,
            },
            .samplers[0] = sg_make_sampler({
                .min_filter = SG_FILTER_LINEAR,
                .mag_filter = SG_FILTER_LINEAR,
                .wrap_u = SG_WRAP_REPEAT,
                .wrap_v = SG_WRAP_REPEAT,
                .label = "model-sampler",
            }),
        },
    };

    batteries::load_img({
        .image_id = material.ao,
        .path = "assets/materials/togezoshell/togezoshell_ao.png",
        .buffer = SG_RANGE(file_buffer),
    });
    batteries::load_img({
        .image_id = material.col,
        .path = "assets/materials/togezoshell/togezoshell_col.png",
        .buffer = SG_RANGE(file_buffer),
    });
    batteries::load_img({
        .image_id = material.mtl,
        .path = "assets/materials/togezoshell/togezoshell_mtl.png",
        .buffer = SG_RANGE(file_buffer),
    });
    batteries::load_img({
        .image_id = material.rgh,
        .path = "assets/materials/togezoshell/togezoshell_rgh.png",
        .buffer = SG_RANGE(file_buffer),
    });
    batteries::load_img({
        .image_id = material.spc,
        .path = "assets/materials/togezoshell/togezoshell_spc.png",
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
}

void Scene::Render(void)
{
    const auto view_proj = camera.projection() * camera.view();

    // initialize uniform data
    const PhysicallyBasedRendering::vs_params_t vs_pbr_params = {
        .view_proj = view_proj,
        .model = togezoshell.transform.matrix(),
    };
    const PhysicallyBasedRendering::fs_params_t fs_pbr_params = {
        .light = light,
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

    sg_begin_pass({.action = pass_action, .attachments = framebuffer.attachments});

    // render using blinn-phong pipeline
    pbr.Render(vs_pbr_params, fs_pbr_params, togezoshell);
    gizmo.Render(vs_gizmo_params, fs_gizmo_params);
    skybox.Render(vs_skybox_params);

    sg_end_pass();

    framebuffer.Render();

    // sg_end_pass();
    // sg_commit();
}