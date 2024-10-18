#pragma once

#include "water.h"

#include "batteries/scene.h"

struct ocean_t
{
  glm::vec3 color;
  glm::vec2 direction;
  float scale;
  float strength;
  float tiling;
  float top_scale;
  float bottom_scale;
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
  Water water;
  Water::water_t water_obj;
  ocean_t ocean;
};
