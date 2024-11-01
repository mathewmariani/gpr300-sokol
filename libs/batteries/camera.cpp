#include "camera.h"

// glm
#define GLM_FORCE_SWIZZLE
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/transform.hpp"

// imgui
#include "imgui/imgui.h"

#include <algorithm>

namespace batteries
{
  static glm::vec3 lerp(const glm::vec3 &a, const glm::vec3 &b, float t)
  {
    return a + t * (b - a);
  }

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
    auto target_position = camera.position; // Start with the current position

    if (move_forward)
    {
      target_position += camera.front * velocity;
    }
    if (move_backward)
    {
      target_position -= camera.front * velocity;
    }
    if (move_left)
    {
      target_position -= camera.right * velocity;
    }
    if (move_right)
    {
      target_position += camera.right * velocity;
    }

    if (!move_forward && !move_backward && !move_left && !move_right)
    {
      t = 0.0f;
    }
    else
    {
      camera.position = lerp(camera.position, target_position, std::min(t + smoothing_factor * dt, 1.0f));
    }

    camera.front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.front.y = sin(glm::radians(pitch));
    camera.front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera.right = glm::normalize(glm::cross(camera.front, camera.world_up));
    camera.up = glm::normalize(glm::cross(camera.right, camera.front));
  }

  void CameraController::Event(const sapp_event *e)
  {
    switch (e->type)
    {
    case SAPP_EVENTTYPE_KEY_DOWN:
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
      break;
    case SAPP_EVENTTYPE_KEY_UP:
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
      break;
    case SAPP_EVENTTYPE_MOUSE_DOWN:
      if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT)
      {
        enable_aim = true;
      }
      break;
    case SAPP_EVENTTYPE_MOUSE_UP:
      if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT)
      {
        enable_aim = false;
      }
      break;
    case SAPP_EVENTTYPE_MOUSE_MOVE:
      if (enable_aim)
      {
        yaw += e->mouse_dx;
        pitch -= e->mouse_dy;
        pitch = glm::clamp(min_pitch, pitch, max_pitch);
      }
      break;
    default:
      break;
    }
  }

  void CameraController::Debug(void)
  {
    ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    // ImGui::Text("Position: %.2f, %.2f, %.2f", camera.position.x, camera.position.y, camera.position.z);
    ImGui::SliderFloat("movement_speed", &movement_speed, 0.0f, 100.0f);
    ImGui::SliderFloat("smoothing_factor", &smoothing_factor, 0.0f, 1.0f);

    ImGui::End();
  }
}