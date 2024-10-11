#pragma once

#include "sokol/sokol_gfx.h"

#include <string>
#include <functional>

#include "fast_obj/fast_obj.h"

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
  void load_texture(const texture_request_t &request);
  void load_cubemap(const cubemap_request_t &request);
}