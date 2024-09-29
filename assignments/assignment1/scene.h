#pragma once

#include "blinnphong.h"

// effects
#include "blur.h"
#include "chromaticaberration.h"
#include "grayscale.h"
#include "inverse.h"

#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/lights.h"
#include "batteries/scene.h"

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

  BlurRenderer blurRenderer;
  ChromaticAberrationRenderer chromaticAberrationRenderer;
  GrayScaleRenderer grayscaleRenderer;
  InverseRenderer inverseRenderer;

  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::model_t suzanne;
  batteries::material_t material;
};
