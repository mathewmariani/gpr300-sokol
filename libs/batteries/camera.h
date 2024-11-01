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
    float aspectRatio = 1.33f;
  };

  class CameraController
  {
  public:
    CameraController();

    void Update(Camera &camera, float dt);
    void Event(const sapp_event *e);

    void Debug(void);

  private:
    // control options
    float movement_speed = 100.0f;
    float smoothing_factor = 1.0f;
    // control attributes
    float yaw = -90.0f;
    float pitch = 0.0f;
    float zoom = 45.0f;
    float min_pitch = -89.0f;
    float max_pitch = 89.0f;
    // control state
    bool enable_aim = false;
    bool move_forward = false;
    bool move_backward = false;
    bool move_left = false;
    bool move_right = false;
    float t = 0.0f;
  };
}