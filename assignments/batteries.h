#ifndef BATTERIES_INCLUDED
#define BATTERIES_INCLUDED

// glm
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>

namespace batteries
{
  struct vertex_t
  {
    glm::vec3 position;
    glm::vec3 normal;
  };

  struct mesh_t
  {
    // sokol resources
    sg_pipeline pip;
    sg_bindings bind;
    sg_buffer vbuf;
    sg_buffer ibuf;

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

  struct model_t
  {
    mesh_t mesh;
    transform_t transform;
  };

  struct camera_t
  {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 target = glm::vec3(0.0f);

    float fov = 60.0f;
    float nearPlane = 0.01f;
    float farPlane = 100.0f;
    bool orthographic = false;
    float orthoHeight = 6.0f;
    float aspectRatio = 1.77f;

    inline glm::mat4 view() const
    {
      return glm::lookAt(position, target, glm::vec3(0, 1, 0));
    }
    inline glm::mat4 projection() const
    {
      return orthographic
                 ? glm::ortho(orthoHeight, aspectRatio, nearPlane, farPlane)
                 : glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }
  };
}

#endif // BATTERIES_INCLUDED
#ifdef BATTERIES_IMPL

namespace batteries
{
}

#endif // BATTERIES_IMPL