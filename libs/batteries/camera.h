#pragma once

// sokol
#include "sokol/sokol_app.h"

// glm
#include "glm/glm.hpp"

namespace batteries
{
  class Camera
  {
  public:
    Camera();

    glm::mat4 View() const;
    glm::mat4 Projection() const;

  public:
    glm::vec3 position;
    glm::vec3 world_up;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

  private:
    bool orthographic = false;
    float fov = 60.0f;
    float nearPlane = 0.01f;
    float farPlane = 1000000.0f;
    float orthoHeight = 6.0f;
    float aspectRatio = 1.77f;
  };

  class CameraController
  {
  public:
    CameraController();

    void Update(Camera &camera, float dt);
    void Event(const sapp_event *e);

  private:
    // control options
    float movement_speed = 0.005f;
    float aim_speed = 1.0f;
    float zoom_speed = 0.1f;
    // control attributes
    float yaw = -90.0f;
    float pitch = 0.0f;
    float zoom = 45.0f;
    float min_pitch = -89.0f;
    float max_pitch = 89.0f;
    // control state
    bool enable_aim;
    bool move_forward;
    bool move_backward;
    bool move_left;
    bool move_right;
  };
}