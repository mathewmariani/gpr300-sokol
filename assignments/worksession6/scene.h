#pragma once

#include "islandpass.h"

#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/lights.h"
#include "batteries/scene.h"
#include "batteries/gizmo.h"

#include <vector>

struct windwaker_model_t
{
  batteries::model_t model;
  std::vector<sg_image> materials;
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
  IslandPass islandPass;
  IslandPass::Palette palette;
  batteries::Gizmo gizmo;

  windwaker_model_t island;
  windwaker_model_t water;

  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::material_t material;
};
