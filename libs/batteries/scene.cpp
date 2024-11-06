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
    cameracontroller.Configure({
        .mode = (int)batteries::CameraController::Mode::Orbit,
        .pitch = 30.0f,
        .yaw = 90.0f,
        .distance = 10.0f,
    });
  }

  Scene::~Scene()
  {
  }

  void Scene::Update(float dt)
  {
    // update internal timer
    time.frame = time.paused ? 0.0f : dt * time.factor;
    if (!time.paused)
    {
      time.absolute += time.frame;
    }

    // update camera
    cameracontroller.Update(dt);
  }

  void Scene::Event(const sapp_event *event)
  {
    cameracontroller.Event(event);
  }
}