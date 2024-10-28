#pragma once

// sokol
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_shape.h"

// batteries
#include "transform.h"

#include <vector>

namespace batteries
{
  struct shape_t
  {
    Transform transform;
    sshape_element_range_t draw;
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
  };

  static shape_t BuildPlane(float width, float height, unsigned short tiles)
  {
    auto info = sshape_plane_sizes(tiles);
    std::vector<sshape_vertex_t> vertices(info.vertices.num);
    std::vector<uint16_t> indices(info.indices.num);

    sshape_buffer_t buf = {
        .vertices.buffer = {vertices.data(), vertices.size() * sizeof(sshape_vertex_t)},
        .indices.buffer = {indices.data(), indices.size() * sizeof(uint16_t)},
    };
    sshape_plane_t plane = {
        .width = width,
        .depth = height,
        .tiles = tiles,
    };

    // one vertex/index-buffer-pair for all shapes
    buf = sshape_build_plane(&buf, &plane);
    const auto vbuf_desc = sshape_vertex_buffer_desc(&buf);
    const auto ibuf_desc = sshape_index_buffer_desc(&buf);

    return (shape_t){
        .transform.position = {0.0f, -1.0f, 0.0f},
        .vertex_buffer = sg_make_buffer(&vbuf_desc),
        .index_buffer = sg_make_buffer(&ibuf_desc),
        .draw = sshape_element_range(&buf),
    };
  }

  static shape_t BuildPlane(void)
  {
    return BuildPlane(400.0f, 400.0f, 10);
  }
}