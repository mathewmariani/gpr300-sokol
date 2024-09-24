#include "scene.h"

namespace batteries
{
  Scene::Scene()
  {
    pass_action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        },
    };

    deferred_action = (sg_pass_action){
        .colors[0].load_action = SG_LOADACTION_LOAD,
        .depth.load_action = SG_LOADACTION_LOAD,
    };
  }

  Scene::~Scene()
  {
  }

  void Scene::Update(float dt)
  {
    camera_controller.update(&camera, dt);
  }

  void Scene::Event(const sapp_event *event)
  {
    camera_controller.event(event);
  }
}