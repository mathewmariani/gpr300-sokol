#pragma once

#include "batteries/framebuffer.h"
#include "batteries/blinnphong.h"
#include "batteries/gizmo.h"
#include "batteries/skybox.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/lights.h"
#include "batteries/scene.h"

class Scene final : public batteries::Scene
{
public:
  Scene();
  virtual ~Scene();

  void Update(float dt);
  void Render();

private:
  batteries::blinnphong_t blinnphong;
  batteries::gizmo_t gizmo;
  batteries::skybox_t skybox;

  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::model_t suzanne;
  batteries::material_t material;
};
