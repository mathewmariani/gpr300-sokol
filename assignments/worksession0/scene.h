#pragma once

#include "pbr.h"

#include "nintendo.h"

#include "batteries/materials.h"
#include "batteries/lights.h"
#include "batteries/scene.h"
#include "batteries/gizmo.h"

class Scene final : public batteries::Scene
{
public:
  Scene();
  virtual ~Scene();

  void Update(float dt);
  void Render(void);
  void Debug(void);

private:
  PhysicallyBasedRendering pbr;
  PhysicallyBasedRendering::Material material;

  nintendo::Model togezoshell;

  batteries::Gizmo gizmo;
  batteries::ambient_t ambient;
  batteries::light_t light;
};
