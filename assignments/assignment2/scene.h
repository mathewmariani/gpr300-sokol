#pragma once

#include "depth.h"
#include "shadow.h"

#include "batteries/gizmo.h"
#include "batteries/skybox.h"
#include "batteries/lights.h"
#include "batteries/scene.h"
#include "batteries/materials.h"

#include "assignment2.h"

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

  assignment2::Model suzanne;
  batteries::Gizmo gizmo;
  batteries::Skybox skybox;

  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::material_t material;
};
