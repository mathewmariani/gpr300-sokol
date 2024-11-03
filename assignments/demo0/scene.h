#pragma once

// batteries
#include "batteries/model.h"
#include "batteries/scene.h"

#include "normals.h"

class Scene final : public batteries::Scene
{
public:
  Scene();
  virtual ~Scene();

  void Update(float dt);
  void Render(void);
  void Debug(void);

private:
  Normals normals;
  batteries::Model suzanne;
};
