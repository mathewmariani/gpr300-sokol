#pragma once

#include "nintendo.h"
#include "toonshading.h"

#include "batteries/gizmo.h"
#include "batteries/lights.h"
#include "batteries/texture.h"
#include "batteries/scene.h"

class Scene final : public batteries::Scene
{
public:
  Scene();
  virtual ~Scene();

  void Update(float dt);
  void Render(void);
  void Debug(void);

private:
  ToonShading toonshading;
  std::vector<nintendo::Model> models;

  batteries::Texture zatoon;
  batteries::Gizmo gizmo;
  batteries::ambient_t ambient;
  batteries::light_t light;
};
