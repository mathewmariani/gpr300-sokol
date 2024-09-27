#include "scene.h"
#include "skybox.h"

namespace batteries
{
  Scene::Scene()
  {
    pass_action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        },
    };

    deferred_action = (sg_pass_action){
        .colors[0].load_action = SG_LOADACTION_LOAD,
        .depth.load_action = SG_LOADACTION_LOAD,
    };

    time = {
        .frame = 0.0,
        .absolute = 0.0,
        .factor = 1.0f,
        .paused = false,
    };
  }

  Scene::~Scene()
  {
  }

  void Scene::Update(float dt)
  {
    // update internal timer
    time.frame = dt;
    if (!time.paused)
    {
      time.absolute += time.frame * time.factor;
    }

    // update camera
    camera_controller.update(&camera, dt);
  }

  void Scene::Begin(void)
  {
    sg_begin_pass({.action = pass_action, .attachments = framebuffer.attachments});
  }

  void Scene::End(void)
  {
    const batteries::Skybox::vs_params_t vs_skybox_params = {
        .view_proj = camera.projection() * glm::mat4(glm::mat3(camera.view())),
    };

    skybox.Render(vs_skybox_params);

    sg_end_pass();

    framebuffer.Render();
  }

  void Scene::Event(const sapp_event *event)
  {
    camera_controller.event(event);
  }
}