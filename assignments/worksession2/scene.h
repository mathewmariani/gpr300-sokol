#pragma once

#include "terrain.h"

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
  Terrain terrain;
  batteries::Texture heightmap;
  batteries::shape_t plane;
};
