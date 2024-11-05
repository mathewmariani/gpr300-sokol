#include "scene.h"
#include "skybox.h"

#include "sokol/sokol_glue.h"

namespace batteries
{
  Scene::Scene()
      : geometrybuffer(800, 600)
  {
    pass = (sg_pass){
        .action = (sg_pass_action){
            .colors[0] = {
                .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
                .load_action = SG_LOADACTION_CLEAR,
            },
            .depth = {
                .clear_value = 1.0f,
                .load_action = SG_LOADACTION_CLEAR,
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

    cameracontroller.SetCamera(&camera);
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
    cameracontroller.Update(dt);
  }

  void Scene::Event(const sapp_event *event)
  {
    cameracontroller.Event(event);
  }
}