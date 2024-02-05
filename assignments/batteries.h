#ifndef BATTERIES_INCLUDED
#define BATTERIES_INCLUDED

// sokol
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_time.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_glue.h"

// libs
#include "imgui/imgui.h"
#include "fast_obj/fast_obj.h"

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

#define MAX_BONES 64
#define MAX_BLEND_SHAPES 64

namespace batteries
{
  constexpr int kilobytes(int n) { return 1024 * n; }
  constexpr int megabytes(int n) { return 1024 * kilobytes(n); }
  constexpr int gigabytes(int n) { return 1024 * megabytes(n); }

  struct vertex_t
  {
    glm::vec3 position;
    glm::vec3 normal;
  };

  struct mesh_t
  {
    int num_faces;
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
  };

  struct material_t
  {
    float Ka, Kd, Ks;
    float Shininess;
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

    void load_obj(const obj_request_t &request);
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

namespace batteries
{
  void setup(void)
  {
    // setup sokol-gfx
    sg_setup((sg_desc){
        .context = sapp_sgcontext(),
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
    struct _obj_request_instance_t
    {
      sg_buffer buffer_id;
      mesh_t *mesh;
    };

    void load_obj(const obj_request_t &request)
    {
      auto obj_callback = [](const sfetch_response_t *response)
      {
        if (response->fetched)
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
        else if (response->failed)
        {
          printf("[!!!] %s\n", __FUNCTION__);
        }
      };

      _obj_request_instance_t _obj = {
          .buffer_id = request.buffer_id,
          .mesh = request.mesh,
      };

      sfetch_send((sfetch_request_t){
          .path = request.path,
          .callback = obj_callback,
          .buffer = {
              .ptr = request.buffer.ptr,
              .size = request.buffer.size,
          },
          .user_data = SFETCH_RANGE(_obj),
      });
    }
  }
}

#endif // BATTERIES_IMPL