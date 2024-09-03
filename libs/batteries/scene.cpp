#include "scene.h"

namespace batteries
{
  Scene::Scene()
  {
    const auto width = sapp_width();
    const auto height = sapp_height();

    batteries::create_framebuffer(&framebuffer, width, height);
  }

  Scene::~Scene()
  {
  }

  void Scene::Update(float dt)
  {
    camera_controller.update(&camera, dt);
  }

  void Scene::Render(void)
  {
  }
}