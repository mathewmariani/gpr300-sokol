#pragma once

// batteries
#include "batteries/scene.h"

// ew
#include "ew/shader.h"

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void Render(void);
    void Event(const sapp_event* event);
    void Debug(void);

  private:
    std::unique_ptr<ew::Shader> splines;
};
