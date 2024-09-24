#pragma once

#include "camera.h"
#include "framebuffer.h"

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

  protected:
    sg_pass_action pass_action;
    sg_pass_action deferred_action;

    batteries::camera_t camera;
    batteries::camera_controller_t camera_controller;

    batteries::Framebuffer framebuffer;
  };
}