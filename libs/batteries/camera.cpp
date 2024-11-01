#include "camera.h"

// glm
#define GLM_FORCE_SWIZZLE
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace batteries
{
  Camera::Camera()
      : position{0.0f, 0.0f, 5.0f}, world_up{0.0f, 1.0f, 0.0f}, front{0.0f, 0.0f, -1.0f}, up{0.0f, 1.0f, 0.0f}, right{1.0f, 0.0f, 0.0f}
  {
  }

  glm::mat4 Camera::View() const
  {
    auto target = position + front;
    return glm::lookAt(position, target, up);
  }

  glm::mat4 Camera::Projection() const
  {
    return orthographic
               ? glm::ortho(orthoHeight, aspectRatio, nearPlane, farPlane)
               : glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
  }

  CameraController::CameraController()
  {
  }

  void CameraController::Update(Camera &camera, float dt)
  {
    auto velocity = movement_speed * dt;
    if (move_forward)
    {
      camera.position += camera.front * velocity;
    }
    if (move_backward)
    {
      camera.position -= camera.front * velocity;
    }
    if (move_left)
    {
      camera.position -= camera.right * velocity;
    }
    if (move_right)
    {
      camera.position += camera.right * velocity;
    }

    camera.front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.front.y = sin(glm::radians(pitch));
    camera.front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.right = glm::normalize(glm::cross(camera.front, camera.world_up));
    camera.up = glm::normalize(glm::cross(camera.right, camera.front));
  }

  void CameraController::Event(const sapp_event *e)
  {
    if (e->type == SAPP_EVENTTYPE_KEY_DOWN)
    {
      if (e->key_code == SAPP_KEYCODE_W || e->key_code == SAPP_KEYCODE_UP)
      {
        move_forward = true;
      }
      else if (e->key_code == SAPP_KEYCODE_S || e->key_code == SAPP_KEYCODE_DOWN)
      {
        move_backward = true;
      }
      else if (e->key_code == SAPP_KEYCODE_A || e->key_code == SAPP_KEYCODE_LEFT)
      {
        move_left = true;
      }
      else if (e->key_code == SAPP_KEYCODE_D || e->key_code == SAPP_KEYCODE_RIGHT)
      {
        move_right = true;
      }
    }
    else if (e->type == SAPP_EVENTTYPE_KEY_UP)
    {
      if (e->key_code == SAPP_KEYCODE_W || e->key_code == SAPP_KEYCODE_UP)
      {
        move_forward = false;
      }
      else if (e->key_code == SAPP_KEYCODE_S || e->key_code == SAPP_KEYCODE_DOWN)
      {
        move_backward = false;
      }
      else if (e->key_code == SAPP_KEYCODE_A || e->key_code == SAPP_KEYCODE_LEFT)
      {
        move_left = false;
      }
      else if (e->key_code == SAPP_KEYCODE_D || e->key_code == SAPP_KEYCODE_RIGHT)
      {
        move_right = false;
      }
    }
    else if (e->type == SAPP_EVENTTYPE_MOUSE_DOWN)
    {
      if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT)
      {
        enable_aim = true;
      }
    }
    else if (e->type == SAPP_EVENTTYPE_MOUSE_UP)
    {
      if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT)
      {
        enable_aim = false;
      }
    }
    if (e->type == SAPP_EVENTTYPE_MOUSE_MOVE)
    {
      if (enable_aim)
      {
        yaw += e->mouse_dx;
        pitch -= e->mouse_dy;
        pitch = glm::clamp(min_pitch, pitch, max_pitch);
      }
    }
  }
}