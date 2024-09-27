#pragma once

#include "transition.h"

#include "batteries/scene.h"

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
  TransitionRenderer transition;
  TransitionRenderer::fs_params_t transition_params;

  gradient_t gradient;
};
