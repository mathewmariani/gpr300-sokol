#pragma once

#include "depth.h"
#include "shadow.h"

#include "batteries/gizmo.h"
#include "batteries/lights.h"
#include "batteries/scene.h"
#include "batteries/materials.h"
#include "batteries/model.h"
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
  Depth depth;
  Shadow shadow;

  batteries::Model suzanne;
  batteries::Gizmo gizmo;

  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::material_t material;
  batteries::shape_t plane;
  batteries::shape_t ortho_wireframe;
};
