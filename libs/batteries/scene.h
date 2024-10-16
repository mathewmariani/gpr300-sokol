#pragma once

#include "camera.h"
#include "framebuffer.h"
#include "skybox.h"

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
    sg_pass pass;

    batteries::camera_t camera;
    batteries::camera_controller_t camera_controller;

    batteries::Skybox skybox;
    batteries::Framebuffer framebuffer;

    struct
    {
      double frame;
      double absolute;
      float factor;
      bool paused;
    } time;
  };
}