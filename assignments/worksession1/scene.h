#pragma once

// batteries
#include "batteries/scene.h"

// ew
#include <memory>

#include "ew/model.h"
#include "ew/shader.h"
#include "ew/texture.h"

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void Render(void);
    void Debug(void);

  private:
    std::unique_ptr<ew::Shader> water;
    std::unique_ptr<ew::Texture> texture;
    std::unique_ptr<ew::Texture> water_mipmap;

    ew::Mesh plane;
};
