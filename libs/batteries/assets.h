#pragma once

// fast_obj
#include "fast_obj/fast_obj.h"

#include <string>

namespace batteries
{
  struct Model;
  struct Mesh;
  struct Texture;

  typedef void (*obj_request_callback_t)(Model *, fastObjMesh *);

  struct Asset
  {
    bool loaded = false;
    Asset() = default;
  };

  struct obj_request_t
  {
    const char *path;
    Model *model;
    obj_request_callback_t callback;
  };
  struct model_request_t
  {
    const char *path;
    Model *model;
  };
  struct texture_request_t
  {
    const char *path;
    Texture *texture;
  };
  struct mipmap_request_t
  {
    struct
    {
      const char *mip0;
      const char *mip1;
      const char *mip2;
      const char *mip3;
      const char *mip4;
      const char *mip5;
      const char *mip6;
      const char *mip7;
    } path;
    Texture *texture;
    uint8_t *buffer_ptr;
    uint32_t buffer_offset;
  };
  struct cubemap_request_t
  {
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
  void load_texture(const texture_request_t &request);
  void load_mipmap(const mipmap_request_t &request);
  void load_cubemap(const cubemap_request_t &request);
}