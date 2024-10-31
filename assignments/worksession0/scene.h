#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/scene.h"
#include "batteries/gizmo.h"
#include "batteries/shape.h"

#include "nintendo.h"
#include "pbr.h"

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
  std::vector<nintendo::Model> models;

  batteries::Gizmo gizmo;
  batteries::Shape sphere;
  batteries::ambient_t ambient;
  batteries::light_t light;
};
