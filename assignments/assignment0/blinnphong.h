#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/pass.h"

class BlinnPhong final : public batteries::Pass
{
public:
  struct vs_params_t
  {
    glm::mat4 view_proj;
    glm::mat4 model;
  };

  struct fs_params_t
  {
    batteries::material_t material;
    batteries::light_t light;
    batteries::ambient_t ambient;
    glm::vec3 camera_position;
  };

public:
  BlinnPhong();

  void Render(const vs_params_t vs_params, const fs_params_t fs_params, batteries::model_t model);
};