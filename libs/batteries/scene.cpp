// batteries
#include "scene.h"

namespace batteries
{
  Scene::Scene()
  {
    time = {
        .frame = 0.0,
        .absolute = 0.0,
        .factor = 1.0f,
        .paused = false,
    };

    cameracontroller.SetCamera(&camera);
    cameracontroller.Configure({
        .mode = (int)batteries::CameraController::Mode::Orbit,
        .distance = 10.0f,
        .yaw = 90.0f,
        .pitch = 30.0f,
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