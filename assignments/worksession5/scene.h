#pragma once

#include "blinnphong.h"
// #include "transition.h"

#include "batteries/scene.h"
#include "batteries/model.h"

struct gradient_t
{
  sg_image img[3];
  sg_sampler smp;
};

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
  // TransitionRenderer transition;
  gradient_t gradient;

  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::material_t material;
  batteries::model_t suzanne;
};
