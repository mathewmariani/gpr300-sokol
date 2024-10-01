#pragma once

// sokol
#include "sokol/sokol_gfx.h"

// fast obj
#include "fast_obj/fast_obj.h"

// batteries
#include "transform.h"

// std
#include <vector>

namespace batteries
{
  // TODO: use vertex_t instead of float
  struct mesh_t
  {
    sg_bindings bindings;
    sg_buffer vbuf;
    sg_buffer ibuf;
    fastObjMesh *obj;
    int num_faces = 0;
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
  };

  struct Mesh
  {
    // sokol info
    sg_bindings bindings;
    sg_buffer vbuf;
    sg_buffer ibuf;
    std::vector<sg_image> textures;
    std::vector<float> vertices;
    std::vector<uint16_t> indices;

    bool loaded = false;

    fastObjMesh *obj;
    int num_faces = 0;

    Mesh() = default;
    static void Load(Mesh *mesh, const void *data, unsigned int size)
    {
      if (!mesh)
      {
        return;
      }

      auto *obj = fast_obj_read((const char *)data, size);
      if (!obj)
      {
        return;
      }

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

      // initialize gfx resources
      mesh->vbuf = sg_alloc_buffer();
      mesh->bindings = (sg_bindings){
          .vertex_buffers[0] = mesh->vbuf,
      };

      // clang-format off
      sg_init_buffer(mesh->vbuf, {
          .data = {
              .ptr = mesh->vertices.data(),
              .size = mesh->vertices.size() * sizeof(float),
          },
          .label = "obj-vertices",
      });
      // clang-format on

      mesh->loaded = true;
    }
  };
}