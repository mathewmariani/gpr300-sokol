#pragma once

#include "blinnphong.h"
#include "transition.h"

#include "batteries/scene.h"
#include "batteries/model.h"

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
  TransitionRenderer transition;

  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::material_t material;
  batteries::model_t suzanne;
};
