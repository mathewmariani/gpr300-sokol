#pragma once


#include "nintendo.h"
#include "pbr.h"

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
  std::vector<nintendo::Model> models;

  batteries::Gizmo gizmo;
  batteries::ambient_t ambient;
  batteries::light_t light;
};
