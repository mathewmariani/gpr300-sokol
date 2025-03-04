#pragma once

// batteries
#include "batteries/scene.h"

// ew
#include "ew/model.h"
#include "ew/shader.h"

class Scene final : public batteries::Scene
{
public:
  Scene();
  virtual ~Scene();

  void Update(float dt);
  void Render(void);
  void Debug(void);

private:
  std::unique_ptr<ew::Model> suzanne;
  std::unique_ptr<ew::Shader> blinnphong;
};
