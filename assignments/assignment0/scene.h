#pragma once

// batteries
#include "batteries/model.h"
#include "batteries/lights.h"
#include "batteries/scene.h"
#include "batteries/shader.h"
#include "batteries/shape.h"

class Scene final : public batteries::Scene
{
public:
  Scene();
  virtual ~Scene();

  void Update(float dt);
  void Render(void);
  void Debug(void);

private:
  batteries::Model suzanne;
  batteries::Shape sphere;

  batteries::ambient_t ambient;
  batteries::light_t light;

  batteries::Shader blinnphong;
};
