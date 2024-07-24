// sokol-fetch
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_fetch.h"

// fast obj
#include "fast_obj/fast_obj.h"

// stb
#include "stb/stb_image.h"

#include "batteries/assets.h"
#include "batteries/model.h"
#include "batteries/mesh.h"

#include <algorithm>
#include <array>
#include <string>
#include <vector>

namespace
{
  struct _obj_request_instance_t
  {
    sg_buffer buffer_id;
    batteries::mesh_t *mesh;
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

        mesh->indices.push_back(i);
      }

      // fast_obj_destroy(obj);
      mesh->obj = obj;

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

namespace batteries {
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
        printf("[!!!] Failed to load object file.\n");
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
        printf("[!!!] Failed to load image file.\n");
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