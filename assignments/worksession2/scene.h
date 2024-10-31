#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/shape.h"
#include "batteries/texture.h"

#include "terrain.h"

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
  batteries::Shape plane;
};
