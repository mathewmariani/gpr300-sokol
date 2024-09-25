#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/pass.h"

struct PhysicallyBasedRendering final : public batteries::Pass
{
  struct Material
  {
    sg_image col;
    sg_image mtl;
    sg_image rgh;
    sg_image ao;
    sg_image spc;
  };

  struct vs_params_t
  {
    glm::mat4 view_proj;
    glm::mat4 model;
  };

  struct fs_params_t
  {
    batteries::light_t light;
    glm::vec3 camera_position;
  };

  PhysicallyBasedRendering();
  void Render(const vs_params_t vs_params, const fs_params_t fs_params, batteries::model_t model);
};