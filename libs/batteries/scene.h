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
    virtual void Render(void);

  protected:
    batteries::camera_t camera;
    batteries::camera_controller_t camera_controller;

    batteries::framebuffer_t framebuffer;
  };
}