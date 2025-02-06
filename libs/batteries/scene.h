#pragma once

#include "camera.h"
#include "depthbuffer.h"
#include "framebuffer.h"
#include "geometrybuffer.h"
#include "hdrbuffer.h"

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

    batteries::Camera camera;
    batteries::CameraController cameracontroller;

    batteries::Framebuffer framebuffer;
    batteries::Depthbuffer depthbuffer;
    batteries::Geometrybuffer geometrybuffer;
    batteries::HDRbuffer hdrbuffer;

    struct
    {
      double frame;
      double absolute;
      float factor;
      bool paused;
    } time;
  };
}