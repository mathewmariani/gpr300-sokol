#pragma once

// batteries
#include "batteries/lights.h"
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
    void RenderTerrain(const glm::vec4 clipping_plane);
    void RenderWater(void);
    void RenderLight(void);
    void RenderSky(void);

    void RecalculateCameraTarget(void);

  private:
    std::unique_ptr<ew::Shader> terrain_shader;
    std::unique_ptr<ew::Shader> water_shader;
    std::unique_ptr<ew::Shader> light_shader;
    std::unique_ptr<ew::Shader> sky_shader;

    std::unique_ptr<ew::Texture> dudv;
    std::unique_ptr<ew::Texture> normal;
    std::unique_ptr<ew::Texture> heightmap;
    std::unique_ptr<ew::Texture> cubemap;

    ew::Mesh terrain_plane;
    ew::Mesh water_plane;
    ew::Mesh light_sphere;
    ew::Mesh sky_box;

    batteries::light_t light;
};
