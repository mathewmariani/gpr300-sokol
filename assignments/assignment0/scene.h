#pragma once

// batteries
#include "batteries/model.h"
#include "batteries/lights.h"
#include "batteries/scene.h"
#include "batteries/gizmo.h"
#include "batteries/shape.h"

#include "blinnphong.h"

class Scene final : public batteries::Scene
{
public:
  Scene();
  virtual ~Scene();

  void Update(float dt);
  void Render(void);
  void Debug(void);

private:
  BlinnPhong blinnphong;
  batteries::Model suzanne;
  batteries::Gizmo gizmo;
  batteries::Shape sphere;

  batteries::ambient_t ambient;
  batteries::light_t light;
};
