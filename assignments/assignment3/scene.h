#pragma once

#include "blinnphong.h"
#include "geometry.h"

#include "batteries/gizmo.h"
#include "batteries/skybox.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/lights.h"
#include "batteries/scene.h"

#include "assignment3.h"

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
  Geometry geometry;

  batteries::Gizmo gizmo;
  batteries::Skybox skybox;
  assignment3::Model suzanne;

  batteries::ambient_t ambient;
  batteries::light_t light;
  batteries::material_t material;
};
