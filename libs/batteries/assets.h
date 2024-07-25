#pragma once

#include "mesh.h"
#include "model.h"

namespace batteries
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
  void load_img(const img_request_t &request);
  void load_cubemap(const cubemap_request_t &request);
}