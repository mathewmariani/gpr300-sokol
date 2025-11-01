#pragma once

// batteries
#include "camera.h"

#include <memory>
#include <vector>

namespace batteries
{
  class Scene
  {
  public:
    Scene();
    virtual ~Scene();

    virtual void Update(float dt);
    virtual void Render(void) {};
    virtual void Debug(void) {};
    virtual void Event(const sapp_event *event);

  protected:
    batteries::Camera camera;
    batteries::CameraController cameracontroller;

    struct
    {
      double frame;
      double absolute;
      float factor;
      bool paused;
    } time;
  };
}