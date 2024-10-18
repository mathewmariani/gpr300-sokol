#pragma once

#include "nintendo.h"

#include "geometry.h"
#include "sea.h"
#include "windowlights.h"

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
  SeaPass seapass;
  WindowLightsPass windowlightspass;

  nintendo::Model island;
  nintendo::Model sea;
  nintendo::Model lights;
};
