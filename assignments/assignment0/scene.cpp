#include "scene.h"

#include "batteries/assets.h"

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};
static uint8_t file_buffer[1024 * 1024 * 5];

Scene::Scene()
{
    batteries::create_skybox_pass(&skybox);
    batteries::create_blinnphong_pass(&blinnphong);
    batteries::create_gizmo_pass(&gizmo);

    ambient = (batteries::ambient_t){
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    light = (batteries::light_t){
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
    };

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
    const batteries::vs_blinnphong_params_t vs_params = {
        .view_proj = view_proj,
        .model = suzanne.transform.matrix(),
    };
    const batteries::fs_blinnphong_params_t fs_params = {
        .material = material,
        .light = light,
        .ambient = ambient,
        .camera_position = camera.position,
    };
    const batteries::vs_gizmo_params_t vs_gizmo_params = {
        .view_proj = view_proj,
        .model = glm::translate(glm::mat4(1.0f), light.position),
    };
    const batteries::fs_gizmo_light_params_t fs_gizmo_light_params = {
        .light_color = light.color,
    };
    const batteries::vs_skybox_params_t vs_skybox_params = {
        .view_proj = camera.projection() * glm::mat4(glm::mat3(camera.view())),
    };

    // apply bindings
    blinnphong.bind = suzanne.mesh.bindings;

    sg_begin_pass({.action = blinnphong.action, .attachments = framebuffer.attachments});

    // render using blinn-phong pipeline
    sg_apply_pipeline(blinnphong.pip);
    sg_apply_bindings(&blinnphong.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
    sg_draw(0, suzanne.mesh.num_faces * 3, 1);

    // render light sources
    sg_apply_pipeline(gizmo.pip);
    sg_apply_bindings(&gizmo.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_gizmo_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_gizmo_light_params));
    sg_draw(gizmo.sphere.base_element, gizmo.sphere.num_elements, 1);

    // render skybox
    sg_apply_pipeline(skybox.pip);
    sg_apply_bindings(&skybox.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_skybox_params));
    sg_draw(0, 36, 1);

    sg_end_pass();

    // render framebuffer
    sg_begin_pass(&framebuffer.pass);
    sg_apply_pipeline(framebuffer.pip);
    sg_apply_bindings(&framebuffer.bind);
    sg_draw(0, 6, 1);

    // sg_end_pass();
    // sg_commit();
}