#pragma once

// sokol
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_shape.h"

// batteries
#include "transform.h"

namespace batteries
{
  struct shape_t
  {
    Transform transform;
    sshape_element_range_t draw;
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
  };

  static shape_t BuildPlane(void)
  {
    // generate shape geometries
    sshape_vertex_t vertices[2 * 1024];
    uint16_t indices[4 * 1024];
    sshape_buffer_t buf = {
        .vertices.buffer = SSHAPE_RANGE(vertices),
        .indices.buffer = SSHAPE_RANGE(indices),
    };
    sshape_plane_t plane = {
        .width = 400.0f,
        .depth = 400.0f,
        .tiles = 10,
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
}