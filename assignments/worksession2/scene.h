#pragma once

// batteries
#include "batteries/scene.h"

// ew
#include "ew/model.h"
#include "ew/shader.h"
#include "ew/texture.h"


#include <memory>

class Scene final : public batteries::Scene
{
public:
  Scene();
  virtual ~Scene();

  void Update(float dt);
  void Render(void);
  void Debug(void);

private:
  std::unique_ptr<ew::Shader> island;
  std::unique_ptr<ew::Texture> heightmap;
  
  ew::Mesh plane;
};
