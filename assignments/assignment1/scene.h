#pragma once

// batteries
#include "batteries/gizmo.h"
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/scene.h"
#include "batteries/shape.h"

#include "blinnphong.h"
#include "blur.h"
#include "chromaticaberration.h"
#include "crt.h"
#include "grayscale.h"
#include "inverse.h"

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
  BlurRenderer blurRenderer;
  ChromaticAberrationRenderer chromaticAberrationRenderer;
  CRTRenderer crtRenderer;
  GrayScaleRenderer grayscaleRenderer;
  InverseRenderer inverseRenderer;

  batteries::Model suzanne;
  batteries::Gizmo gizmo;
  batteries::Shape sphere;
  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::material_t material;
};
