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

  static shape_t BuildPlane(float width, float height, uint16_t tiles)
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
    };

    buf = sshape_build_plane(&buf, &plane);
    return (shape_t){
        .transform.position = {0.0f, -1.0f, 0.0f},
        .vertex_buffer = sg_make_buffer(sshape_vertex_buffer_desc(&buf)),
        .index_buffer = sg_make_buffer(sshape_index_buffer_desc(&buf)),
        .draw = sshape_element_range(&buf),
    };
  }

  static shape_t BuildBox(float width, float height, float depth, uint16_t tiles)
  {
    auto info = sshape_box_sizes(tiles);
    std::vector<sshape_vertex_t> vertices(info.vertices.num);
    std::vector<uint16_t> indices(info.indices.num);

    sshape_buffer_t buf = {
        .vertices.buffer = {vertices.data(), vertices.size() * sizeof(sshape_vertex_t)},
        .indices.buffer = {indices.data(), indices.size() * sizeof(uint16_t)},
    };
    sshape_box_t box = {
        .width = width,
        .height = height,
        .depth = depth,
        .tiles = tiles,
    };

    buf = sshape_build_box(&buf, &box);
    return (shape_t){
        .transform.position = {0.0f, -1.0f, 0.0f},
        .vertex_buffer = sg_make_buffer(sshape_vertex_buffer_desc(&buf)),
        .index_buffer = sg_make_buffer(sshape_index_buffer_desc(&buf)),
        .draw = sshape_element_range(&buf),
    };
  }

  static shape_t BuildSphere(float radius, uint16_t slices, uint16_t stacks)
  {
    auto info = sshape_sphere_sizes(slices, stacks);
    std::vector<sshape_vertex_t> vertices(info.vertices.num);
    std::vector<uint16_t> indices(info.indices.num);

    sshape_buffer_t buf = {
        .vertices.buffer = {vertices.data(), vertices.size() * sizeof(sshape_vertex_t)},
        .indices.buffer = {indices.data(), indices.size() * sizeof(uint16_t)},
    };
    sshape_sphere_t box = {
        .radius = radius,
        .slices = slices,
        .stacks = stacks,
    };

    buf = sshape_build_sphere(&buf, &box);
    return (shape_t){
        .transform.position = {0.0f, -1.0f, 0.0f},
        .vertex_buffer = sg_make_buffer(sshape_vertex_buffer_desc(&buf)),
        .index_buffer = sg_make_buffer(sshape_index_buffer_desc(&buf)),
        .draw = sshape_element_range(&buf),
    };
  }

  static shape_t BuildPlane(void)
  {
    return BuildPlane(400.0f, 400.0f, 10);
  }
}