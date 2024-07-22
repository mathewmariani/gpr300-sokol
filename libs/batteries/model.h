#pragma once

// sokol
#include "sokol/sokol_gfx.h"

// glm
// FIXME: remove this
#define GLM_FORCE_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/transform.hpp"

// fast obj
#include "fast_obj/fast_obj.h"

#include <vector>

namespace batteries
{
  struct vertex_t
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
  };

  // TODO: use vertex_t instead of float
  struct mesh_t
  {
    sg_buffer vbuf;
    sg_buffer ibuf;
    fastObjMesh *obj;
    int num_faces;
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
  };

  struct transform_t
  {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::mat4 matrix() const
    {
      glm::mat4 m = glm::mat4(1.0f);
      m = glm::scale(m, scale);
      m *= glm::mat4_cast(rotation);
      m = glm::translate(m, position);
      return m;
    }
  };

  struct shape_t
  {
    transform_t transform;
    sshape_element_range_t draw;
  };

  struct model_t
  {
    mesh_t mesh;
    transform_t transform;
  };
}