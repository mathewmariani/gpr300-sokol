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
    std::unique_ptr<ew::Shader> blinnphong;
    std::unique_ptr<ew::Shader> depth;
    std::unique_ptr<ew::Shader> snow;

    std::unique_ptr<ew::Model> suzanne;
    std::unique_ptr<ew::Texture> snow_texture;

    batteries::ambient_t ambient;
    batteries::light_t light;

    ew::Mesh plane;
};
