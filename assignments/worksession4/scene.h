#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/scene.h"

// ew
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
    std::unique_ptr<ew::Model> skull;
    std::unique_ptr<ew::Shader> toonshading;
    std::unique_ptr<ew::Texture> texture;
    std::unique_ptr<ew::Texture> zatoon;

    batteries::ambient_t ambient;
    batteries::light_t light;
};
