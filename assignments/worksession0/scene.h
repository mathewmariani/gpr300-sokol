#pragma once

#include "pbr.h"

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
  PhysicallyBasedRendering pbr;

  batteries::Gizmo gizmo;
  batteries::Skybox skybox;
  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::model_t togezoshell;
  PhysicallyBasedRendering::Material material;
};