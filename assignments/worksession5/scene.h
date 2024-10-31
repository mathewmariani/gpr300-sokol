#pragma once

// batteries
#include "batteries/gizmo.h"
#include "batteries/scene.h"
#include "batteries/model.h"
#include "batteries/texture.h"
#include "batteries/shape.h"

#include "worksession5.h"
#include "blinnphong.h"
#include "transition.h"

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
  batteries::Shape sphere;
  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::material_t material;
};
