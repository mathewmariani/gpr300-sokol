#include "scene.h"

namespace batteries
{
  Scene::Scene()
  {
    const auto width = sapp_width();
    const auto height = sapp_height();

    batteries::create_framebuffer(&framebuffer, width, height);

    pass_action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        },
    };
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