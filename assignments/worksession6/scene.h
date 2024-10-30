#pragma once

#include "nintendo.h"

#include "geometry.h"
#include "lights.h"
#include "water.h"

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
  GeometryPass geometrypass;
  LightsPass lightspass;
  WaterPass waterpass;

  nintendo::Model island;
  nintendo::Model sea;
  nintendo::Model lights;
};
