#pragma once

// batteries
#include "batteries/lights.h"
#include "batteries/materials.h"
#include "batteries/model.h"
#include "batteries/pass.h"
#include "batteries/gbuffer.h"

#define MAX_LIGHTS (64)

struct BlinnPhong final : public batteries::Pass
{
  struct my_light_t
  {
    glm::vec4 color[MAX_LIGHTS];
    glm::vec4 position[MAX_LIGHTS];
  };

  struct fs_params_t
  {
    glm::vec3 camera_position;
    my_light_t lights;
    batteries::ambient_t ambient;
    int num_instances;
  };

  BlinnPhong(const batteries::Gbuffer &buffer);
  void Render(const fs_params_t fs_params);
};