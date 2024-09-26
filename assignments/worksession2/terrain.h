#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/pass.h"
#include "batteries/shape.h"

struct Terrain final : public batteries::Pass
{
  struct plane_t
  {
    sg_bindings bind;
    sg_image img;
    batteries::shape_t plane;
  };

  struct vs_params_t
  {
    glm::mat4 view_proj;
    glm::mat4 model;
    glm::vec3 camera_pos;
    float scale;
  };

  Terrain();
  void Render(const vs_params_t vs_params, Terrain::plane_t plane);
};