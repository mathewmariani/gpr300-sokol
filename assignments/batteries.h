#ifndef BATTERIES_INCLUDED
#define BATTERIES_INCLUDED

// sokol
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_time.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_shape.h"

// libs
#include "imgui/imgui.h"

// glm
#define GLM_FORCE_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/transform.hpp"

#include <algorithm>
#include <array>
#include <string>
#include <vector>

namespace batteries
{
  constexpr int kilobytes(int n) { return 1024 * n; }
  constexpr int megabytes(int n) { return 1024 * kilobytes(n); }
  constexpr int gigabytes(int n) { return 1024 * megabytes(n); }

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
    int num_faces;
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
  };

  struct material_t
  {
    float Ka, Kd, Ks;
    float Shininess;
  };

  struct ambient_t
  {
    glm::vec3 direction;
    glm::vec3 color;
  };

  struct pointlight_t
  {
    float radius;
    glm::vec4 color;
    glm::vec4 position;
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
    float farPlane = 100.0f;
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

  struct shape_t
  {
    transform_t transform;
    sshape_element_range_t draw;
  };

  void setup(void);
  void shutdown(void);
  void frame(void);
  void event(const sapp_event *event);

  namespace assets
  {
    struct obj_request_t
    {
      sg_buffer buffer_id;
      sg_range buffer;
      const char *path;
      mesh_t *mesh;
    };

    struct img_request_t
    {
      sg_image image_id;
      sg_range buffer;
      const char *path;
    };

    struct cubemap_request_t
    {
      sg_image img_id;
      const char *path_right;
      const char *path_left;
      const char *path_top;
      const char *path_bottom;
      const char *path_front;
      const char *path_back;
      uint8_t *buffer_ptr;
      uint32_t buffer_offset;
    };

    void load_obj(const obj_request_t &request);
    void load_img(const obj_request_t &request);
    void load_cubemap(const cubemap_request_t &request);
  }
}

#endif // BATTERIES_INCLUDED
#ifdef BATTERIES_IMPL

// sokol-fetch
#include "sokol/sokol_fetch.h"

// sokol-imgui
#define SOKOL_IMGUI_IMPL
#include "sokol/sokol_imgui.h"

// stb
#include "stb/stb_image.h"

// fast_obj
#include "fast_obj/fast_obj.h"

namespace batteries
{
  void setup(void)
  {
    // setup sokol-gfx
    sg_setup((sg_desc){
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });

    // setup sokol-time
    stm_setup();

    // setup sokol-fetch
    sfetch_setup((sfetch_desc_t){
        .max_requests = 8,
        .num_channels = 1,
        .num_lanes = 1,
        .logger.func = slog_func,
    });

    // setup sokol-imgui
    simgui_setup((simgui_desc_t){
        .logger.func = slog_func,
    });

    stbi_set_flip_vertically_on_load(true);
  }

  void shutdown(void)
  {
    sfetch_shutdown();
    simgui_shutdown();
    sg_shutdown();
  }

  void frame(void)
  {
    sfetch_dowork();
    simgui_new_frame({sapp_width(), sapp_height(), sapp_frame_duration(), sapp_dpi_scale()});
  }

  void event(const sapp_event *event)
  {
    simgui_handle_event(event);
  }

  namespace assets
  {
    namespace
    {
      struct _obj_request_instance_t
      {
        sg_buffer buffer_id;
        mesh_t *mesh;
      };
      struct _img_request_instance_t
      {
        sg_image image_id;
      };
      struct _cubemap_request_t
      {
        sg_image img_id;
        uint8_t *buffer;
        int buffer_offset;
        int fetched_sizes[6];
        int finished_requests;
        bool failed;
      };
      struct _cubemap_request_instance_t
      {
        int index;
        _cubemap_request_t *request;
      };

      // TODO: use index/element buffer:
      void read_obj(const sfetch_response_t *response)
      {
        auto *obj = fast_obj_read((const char *)response->data.ptr, response->data.size);
        if (obj)
        {
          auto req_inst = (_obj_request_instance_t *)response->user_data;
          auto *mesh = req_inst->mesh;
          mesh->num_faces = obj->face_count;
          for (auto i = 0; i < obj->face_count * 3; ++i)
          {
            auto vertex = obj->indices[i];
            // vertex
            mesh->vertices.push_back(*((obj->positions + vertex.p * 3) + 0));
            mesh->vertices.push_back(*((obj->positions + vertex.p * 3) + 1));
            mesh->vertices.push_back(*((obj->positions + vertex.p * 3) + 2));
            // normals
            mesh->vertices.push_back(*((obj->normals + vertex.n * 3) + 0));
            mesh->vertices.push_back(*((obj->normals + vertex.n * 3) + 1));
            mesh->vertices.push_back(*((obj->normals + vertex.n * 3) + 2));
            // texcoords
            mesh->vertices.push_back(*((obj->texcoords + vertex.t * 2) + 0));
            mesh->vertices.push_back(*((obj->texcoords + vertex.t * 2) + 1));
          }

          fast_obj_destroy(obj);

          // clang-format off
          sg_init_buffer(req_inst->buffer_id, (sg_buffer_desc){
              .data = {
                  .ptr = mesh->vertices.data(),
                  .size = mesh->vertices.size() * sizeof(float),
              },
              .label = "obj-vertices",
          });
          // clang-format on
        }
      }
      void read_img(const sfetch_response_t *response)
      {
        int width, height, components;
        auto *pixels = stbi_load_from_memory(
            (const stbi_uc *)response->data.ptr,
            response->data.size,
            &width,
            &height,
            &components,
            4);

        if (pixels)
        {
          auto req_inst = (_img_request_instance_t *)response->user_data;
          // clang-format off
          sg_init_image(req_inst->image_id, (sg_image_desc){
              .type = SG_IMAGETYPE_2D,
              .width = width,
              .height = height,
              .pixel_format = SG_PIXELFORMAT_RGBA8,
              .data.subimage[0][0] = {
                  .ptr = pixels,
                  .size = (size_t)(width * height * 4),
              }
          });
          // clang-format on
        }

        stbi_image_free(pixels);
      }
      void read_cubemap(const sfetch_response_t *response)
      {
        auto req_inst = *(_cubemap_request_instance_t *)response->user_data;
        auto *request = req_inst.request;

        // TODO: this should be seperated into a load texture
        const int desired_channels = 4;
        int img_widths[6], img_heights[6];
        stbi_uc *pixels_ptrs[6];
        sg_image_data img_content;
        for (auto i = 0; i < 6; ++i)
        {
          pixels_ptrs[i] = stbi_load_from_memory(
              request->buffer + (i * request->buffer_offset),
              request->fetched_sizes[i],
              &img_widths[i],
              &img_heights[i],
              nullptr,
              desired_channels);

          img_content.subimage[i][0].ptr = pixels_ptrs[i];
          img_content.subimage[i][0].size = img_widths[i] * img_heights[i] * desired_channels;
        }

        // clang-format off
        sg_init_image(request->img_id, (sg_image_desc){
            .type = SG_IMAGETYPE_CUBE,
            .width = img_widths[0],
            .height = img_heights[0],
            .pixel_format = SG_PIXELFORMAT_RGBA8,
            .data = img_content,
        });
        // clang-format on

        for (auto i = 0; i < 6; ++i)
        {
          stbi_image_free(pixels_ptrs[i]);
        }
      }
    }

    static struct
    {
      _cubemap_request_t cubemap_req;
    } _state;

    void load_obj(const obj_request_t &request)
    {
      auto obj_fetch_callback = [](const sfetch_response_t *response)
      {
        if (response->fetched)
        {
          read_obj(response);
        }
        else if (response->failed)
        {
          printf("[!!!] Failed to load .obj file.\n");
        }
      };

      _obj_request_instance_t _obj = {
          .buffer_id = request.buffer_id,
          .mesh = request.mesh,
      };

      sfetch_send((sfetch_request_t){
          .path = request.path,
          .callback = obj_fetch_callback,
          .buffer = {
              .ptr = request.buffer.ptr,
              .size = request.buffer.size,
          },
          .user_data = SFETCH_RANGE(_obj),
      });
    }
    void load_img(const img_request_t &request)
    {
      auto img_fetch_callback = [](const sfetch_response_t *response)
      {
        if (response->fetched)
        {
          read_img(response);
        }
        else if (response->failed)
        {
          printf("[!!!] Failed to load .obj file.\n");
        }
      };

      _img_request_instance_t _obj = {
          .image_id = request.image_id,
      };

      sfetch_send((sfetch_request_t){
          .path = request.path,
          .callback = img_fetch_callback,
          .buffer = {
              .ptr = request.buffer.ptr,
              .size = request.buffer.size,
          },
          .user_data = SFETCH_RANGE(_obj),
      });
    }
    void load_cubemap(const cubemap_request_t &request)
    {
      auto cubemap_fetch_callback = [](const sfetch_response_t *response)
      {
        _cubemap_request_instance_t req_inst = *(_cubemap_request_instance_t *)response->user_data;
        _cubemap_request_t *request = req_inst.request;

        if (response->fetched)
        {
          request->fetched_sizes[req_inst.index] = response->data.size;
          ++request->finished_requests;
        }
        else if (response->failed)
        {
          request->failed = true;
          ++request->finished_requests;
        }

        if (request->finished_requests == 6)
        {
          if (!request->failed)
          {
            read_cubemap(response);
          }
          else
          {
            printf("[!!!] %s\n", __FUNCTION__);
          }
        }
      };

      // FIXME: what is this, and why is it important if even?
      _state.cubemap_req = (_cubemap_request_t){
          .img_id = request.img_id,
          .buffer = request.buffer_ptr,
          .buffer_offset = (int)request.buffer_offset,
      };

      std::array<std::string, 6> cubemap = {
          request.path_right,
          request.path_left,
          request.path_top,
          request.path_bottom,
          request.path_front,
          request.path_back,
      };

      for (auto i = 0; i < 6; ++i)
      {
        _cubemap_request_instance_t req_instance = {
            .index = i,
            .request = &_state.cubemap_req,
        };
        sfetch_send((sfetch_request_t){
            .path = cubemap.at(i).c_str(),
            .callback = cubemap_fetch_callback,
            .buffer = {
                .ptr = request.buffer_ptr + (i * request.buffer_offset),
                .size = request.buffer_offset,
            },
            .user_data = SFETCH_RANGE(req_instance),
        });
      }
    }
  }
}

#endif // BATTERIES_IMPL