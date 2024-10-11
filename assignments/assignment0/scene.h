#pragma once

#include "blinnphong.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/lights.h"
#include "batteries/scene.h"
#include "batteries/gizmo.h"

#include "assignment0.h"

class Scene final : public batteries::Scene
{
public:
  Scene();
  virtual ~Scene();

  void Update(float dt);
  void Render(void);
  void Debug(void);

private:
  BlinnPhong blinnPhong;
  assignment0::Model suzanne;
  batteries::Gizmo gizmo;

  batteries::ambient_t ambient;
  batteries::light_t light;
};
