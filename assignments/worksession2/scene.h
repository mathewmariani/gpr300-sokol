#pragma once

#include "terrain.h"

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
  Terrain terrain;
  Terrain::plane_t terrain_obj;
};
