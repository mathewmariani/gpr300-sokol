#pragma once

#include "depth.h"
#include "shadow.h"

#include "batteries/depthbuffer.h"
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
  Depth depth;
  Shadow shadow;

  batteries::Depthbuffer depthbuffer;
  batteries::Gizmo gizmo;
  batteries::Skybox skybox;

  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::model_t suzanne;
  batteries::material_t material;
};
