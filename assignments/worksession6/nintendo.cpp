#include "nintendo.h"

// batteries
#include "batteries/vertex.h"

// fastobj
#include "fast_obj/fast_obj.h"

namespace nintendo
{
  static void fetch_callback(batteries::Model *model, fastObjMesh *obj)
  {
    batteries::Mesh *mesh = &model->mesh;
    model->loaded = true;

    mesh->num_faces = obj->face_count;
    for (auto i = 0; i < obj->face_count * 3; ++i)
    {
      auto indices = obj->indices[i];
      mesh->vertices.push_back({
          .position{
              *((obj->positions + indices.p * 3) + 0),
              *((obj->positions + indices.p * 3) + 1),
              *((obj->positions + indices.p * 3) + 2),
          },
          .normal = {
              *((obj->normals + indices.n * 3) + 0),
              *((obj->normals + indices.n * 3) + 1),
              *((obj->normals + indices.n * 3) + 2),
          },
          .texcoord = {
              *((obj->texcoords + indices.t * 2) + 0),
              *((obj->texcoords + indices.t * 2) + 1),
          },
      });
      mesh->indices.push_back(i);
    }

    for (auto i = 0; i < obj->group_count; ++i)
    {
      auto group = obj->groups[i];
      mesh->groups.push_back({
          .face_count = group.face_count,
          .face_offset = group.face_offset,
          .index_offset = group.index_offset,
      });
    }

    // FIXME: load materials from lib
    ((nintendo::Model *)model)->textures.resize(obj->material_count);
    for (auto i = 0; i < obj->material_count; ++i)
    {
      if (obj->materials[i].map_Kd.name == 0)
      {
        continue;
      }

      ((nintendo::Model *)model)->textures[i].Load(obj->materials[i].map_Kd.name);
    }

    // initialize gfx resources
    mesh->sampler = sg_make_sampler({
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .wrap_u = SG_WRAP_REPEAT,
        .wrap_v = SG_WRAP_REPEAT,
        .label = "model-sampler",
    });
    mesh->vertex_buffer = sg_make_buffer({
        .type = SG_BUFFERTYPE_VERTEXBUFFER,
        .data = {
            .ptr = mesh->vertices.data(),
            .size = mesh->vertices.size() * sizeof(batteries::vertex_t),
        },
        .label = "mesh-vertices",
    });
    mesh->index_buffer = sg_make_buffer({
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = {
            .ptr = mesh->indices.data(),
            .size = mesh->indices.size() * sizeof(uint16_t),
        },
        .label = "mesh-indices",
    });
  }
  void Model::Load(const std::string &path)
  {
    batteries::load_obj({
        .path = path.c_str(),
        .callback = fetch_callback,
        .model = this,
    });
  }
}