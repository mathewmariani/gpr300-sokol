#pragma once

#include "worksession5.h"
#include "blinnphong.h"
#include "transition.h"

#include "batteries/gizmo.h"
#include "batteries/scene.h"
#include "batteries/model.h"
#include "batteries/texture.h"

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
  TransitionRenderer transition;

  worksession5::Model suzanne;
  batteries::Texture gradients[3];
  batteries::Gizmo gizmo;
  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::material_t material;
};
