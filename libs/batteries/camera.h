#pragma once

// sokol
#include "sokol/sokol_app.h"

// glm
#define GLM_FORCE_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace batteries
{
  struct camera_t
  {
    // camera attributes
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    // internal state
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    // old but can be useful
    float fov = 60.0f;
    float nearPlane = 0.01f;
    float farPlane = 1000000.0f;
    bool orthographic = false;
    float orthoHeight = 6.0f;
    float aspectRatio = 1.77f;

    inline glm::mat4 view() const
    {
      auto target = position + front;
      return glm::lookAt(position, target, up);
    }
    inline glm::mat4 projection() const
    {
      return orthographic
                  ? glm::ortho(orthoHeight, aspectRatio, nearPlane, farPlane)
                  : glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }
  };

  struct camera_controller_t
  {
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

    void update(camera_t *camera, float dt)
    {
      auto velocity = movement_speed * (dt * 1000);
      if (move_forward)
      {
        camera->position += camera->front * velocity;
      }
      if (move_backward)
      {
        camera->position -= camera->front * velocity;
      }
      if (move_left)
      {
        camera->position -= camera->right * velocity;
      }
      if (move_right)
      {
        camera->position += camera->right * velocity;
      }

      camera->front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
      camera->front.y = sin(glm::radians(pitch));
      camera->front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
      camera->right = glm::normalize(glm::cross(camera->front, camera->world_up));
      camera->up = glm::normalize(glm::cross(camera->right, camera->front));
    }

    void event(const sapp_event *e)
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
  };
}