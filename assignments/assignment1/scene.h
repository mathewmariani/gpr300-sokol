#pragma once

#include "assignment1.h"
#include "blinnphong.h"

// effects
#include "blur.h"
#include "chromaticaberration.h"
#include "grayscale.h"
#include "inverse.h"

// batteries
#include "batteries/gizmo.h"
#include "batteries/lights.h"
#include "batteries/materials.h"
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
  BlinnPhong blinnphong;
  assignment1::Model suzanne;

  BlurRenderer blurRenderer;
  ChromaticAberrationRenderer chromaticAberrationRenderer;
  GrayScaleRenderer grayscaleRenderer;
  InverseRenderer inverseRenderer;

  batteries::Gizmo gizmo;
  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::material_t material;
};
