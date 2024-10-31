#pragma once

#include "water.h"

#include "batteries/scene.h"
#include "batteries/shape.h"

class Scene final : public batteries::Scene
{
public:
  Scene();
  virtual ~Scene();

  void Update(float dt);
  void Render(void);
  void Debug(void);

private:
  Water water;
  batteries::Shape plane;
};
