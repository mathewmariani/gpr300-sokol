// sokol-fetch
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_fetch.h"

// stb
#include "stb/stb_image.h"

// fast_obj
#include "fast_obj/fast_obj.h"

#include "batteries/assets.h"
#include "batteries/model.h"
#include "batteries/mesh.h"
#include "batteries/texture.h"

#include <algorithm>
#include <array>
#include <string>
#include <vector>

namespace
{
  static uint8_t file_buffer[1024 * 1024 * 10];
  struct obj_request_data_t
  {
    fastObjMesh *mesh;
    batteries::Model *model;
    batteries::obj_request_callback_t callback;
  };
  struct _texture_request_instance_t
  {
    batteries::Texture *texture;
  };
  struct _mipmap_request_t
  {
    sg_image img_id;
    uint8_t *buffer;
    int buffer_offset;
    int fetched_sizes[8];
    int finished_requests;
    bool failed;
  };
  struct _mipmap_request_instance_t
  {
    int index;
    _mipmap_request_t *request;
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

  void read_texture(batteries::Texture *texture, const void *data, unsigned int size)
  {
    int width, height, components;
    auto *pixels = stbi_load_from_memory((const stbi_uc *)data, size, &width, &height, &components, 4);

    if (!pixels)
    {
      return;
    }
    texture->loaded = true;

    // clang-format off
    sg_init_image(texture->image, {
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
  void read_mipmap(const sfetch_response_t *response)
  {
    auto req_inst = *(_mipmap_request_instance_t *)response->user_data;
    auto *request = req_inst.request;

    // TODO: this should be seperated into a load texture
    const int desired_channels = 4;
    int img_widths[8], img_heights[8];
    stbi_uc *pixels_ptrs[8];
    sg_image_data img_content;
    for (auto i = 0; i < 8; ++i)
    {
      pixels_ptrs[i] = stbi_load_from_memory(
          request->buffer + (i * request->buffer_offset),
          request->fetched_sizes[i],
          &img_widths[i],
          &img_heights[i],
          nullptr,
          desired_channels);

      img_content.subimage[0][i].ptr = pixels_ptrs[i];
      img_content.subimage[0][i].size = img_widths[i] * img_heights[i] * desired_channels;
    }

    // clang-format off
    sg_init_image(request->img_id, (sg_image_desc){
        .type = SG_IMAGETYPE_2D,
        .width = img_widths[0],
        .height = img_heights[0],
        .num_mipmaps = 5,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .data = img_content,
    });
    // clang-format on

    for (auto i = 0; i < 8; ++i)
    {
      stbi_image_free(pixels_ptrs[i]);
    }
  }
}

namespace batteries
{
  static struct
  {
    _cubemap_request_t cubemap_req;
    _mipmap_request_t mipmap_req;
  } _state;

  static void mtl_fetch_callback(const sfetch_response_t *response)
  {
    obj_request_data_t request = *(obj_request_data_t *)response->user_data;
    if (response->fetched)
    {
      fast_obj_mtllib_read(request.mesh, (const char *)response->data.ptr, response->data.size);
      request.callback(request.model, request.mesh);
    }
    else if (response->failed)
    {
      printf("[!!!] Failed to load mtl file.\n");
    }
    fast_obj_destroy(request.mesh);
  }
  static void obj_fetch_callback(const sfetch_response_t *response)
  {
    if (response->fetched)
    {
      obj_request_data_t request = *(obj_request_data_t *)response->user_data;
      request.mesh = fast_obj_read((const char *)response->data.ptr, response->data.size);

      if (request.mesh->mtllib_count == 0)
      {
        request.callback(request.model, request.mesh);
        return;
      }
      for (auto i = 0; i < request.mesh->mtllib_count; ++i)
      {
        sfetch_send({
            .path = request.mesh->mtllibs[i],
            .callback = mtl_fetch_callback,
            .buffer = SFETCH_RANGE(file_buffer),
            .user_data = SFETCH_RANGE(request),
        });
      }
    }
    else if (response->failed)
    {
      printf("[!!!] Failed to load obj file.\n");
    }
  }

  void load_obj(const obj_request_t &request)
  {
    obj_request_data_t wrapper = {
        .mesh = nullptr,
        .model = request.model,
        .callback = request.callback,
    };

    sfetch_send({
        .path = request.path,
        .callback = obj_fetch_callback,
        .buffer = SFETCH_RANGE(file_buffer),
        .user_data = SFETCH_RANGE(wrapper),
    });
  }
  void load_texture(const texture_request_t &request)
  {
    auto fetch_callback = [](const sfetch_response_t *response)
    {
      if (response->fetched)
      {
        auto *texture = ((_texture_request_instance_t *)response->user_data)->texture;
        read_texture(texture, response->data.ptr, response->data.size);
      }
      else if (response->failed)
      {
        printf("[!!!] Failed to load texture.\n");
      }
    };

    _texture_request_instance_t wrapper = {
        .texture = request.texture,
    };

    sfetch_send({
        .path = request.path,
        .callback = fetch_callback,
        .buffer = SFETCH_RANGE(file_buffer),
        .user_data = SFETCH_RANGE(wrapper),
    });
  }
  void load_mipmap(const mipmap_request_t &request)
  {
    auto mipmap_fetch_callback = [](const sfetch_response_t *response)
    {
      _mipmap_request_instance_t req_inst = *(_mipmap_request_instance_t *)response->user_data;
      _mipmap_request_t *request = req_inst.request;

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

      if (request->finished_requests == 5)
      {
        if (!request->failed)
        {
          read_mipmap(response);
        }
        else
        {
          printf("[!!!] %s\n", __FUNCTION__);
        }
      }
    };

    // FIXME: what is this, and why is it important if even?
    _state.mipmap_req = (_mipmap_request_t){
        .img_id = request.img_id,
        .buffer = request.buffer_ptr,
        .buffer_offset = (int)request.buffer_offset,
    };

    const char *paths[8]{
        request.path.mip0,
        request.path.mip1,
        request.path.mip2,
        request.path.mip3,
        request.path.mip4,
        request.path.mip5,
        request.path.mip6,
        request.path.mip7,
    };

    for (auto i = 0; i < 8; ++i)
    {
      _mipmap_request_instance_t req_instance = {
          .index = i,
          .request = &_state.mipmap_req,
      };
      if (paths[i] == nullptr)
      {
        continue;
      }
      sfetch_send((sfetch_request_t){
          .path = paths[i],
          .callback = mipmap_fetch_callback,
          .buffer = {
              .ptr = request.buffer_ptr + (i * request.buffer_offset),
              .size = request.buffer_offset,
          },
          .user_data = SFETCH_RANGE(req_instance),
      });
    }
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