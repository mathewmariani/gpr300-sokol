#include "scene.h"
#include "skybox.h"

#include "sokol/sokol_glue.h"

namespace batteries
{
  Scene::Scene()
  {
    pass = (sg_pass){
        .action = (sg_pass_action){
            .colors[0] = {
                .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
                .load_action = SG_LOADACTION_CLEAR,
            },
            .depth = {
                .load_action = SG_LOADACTION_CLEAR,
                .store_action = SG_STOREACTION_STORE,
                .clear_value = 1.0f,
            },
        },
        .swapchain = sglue_swapchain(),
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

  void Scene::Event(const sapp_event *event)
  {
    camera_controller.event(event);
  }
}