#pragma once

// batteries
#include "batteries/gizmo.h"
#include "batteries/lights.h"
#include "batteries/scene.h"
#include "batteries/model.h"
#include "batteries/shape.h"
#include "batteries/texture.h"

#include "depth.h"
#include "shadow.h"
#include "dungeon.h"

class Scene final : public batteries::Scene
{
public:
  Scene();
  virtual ~Scene();

  void Update(float dt);
  void Render(void);
  void Debug(void);

private:
  Depth depth;
  Shadow shadow;
  Dungeon dungeon;

  batteries::Model suzanne;
  batteries::Gizmo gizmo;
  batteries::Shape cube;
  batteries::Shape sphere;
  batteries::Texture dungeon_texture;

  batteries::ambient_t ambient;
  batteries::light_t light;
};
