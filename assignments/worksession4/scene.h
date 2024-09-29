#pragma once

#include "toonshading.h"

#include "batteries/gizmo.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/lights.h"
#include "batteries/scene.h"

struct skull_t
{
  sg_image albedo;
  sg_image zatoon;
  batteries::model_t model;
};

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
  ToonShading::Palette palette;
  skull_t skull;
  batteries::Gizmo gizmo;

  batteries::ambient_t ambient;
  batteries::light_t light;
};
