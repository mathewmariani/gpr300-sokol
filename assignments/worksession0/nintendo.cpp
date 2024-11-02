#include "nintendo.h"

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

    ((nintendo::Model *)model)->ao.Load(obj->materials[0].map_Ka.name);
    ((nintendo::Model *)model)->col.Load(obj->materials[0].map_Kd.name);
    ((nintendo::Model *)model)->mtl.Load(obj->materials[0].map_Ks.name);
    ((nintendo::Model *)model)->rgh.Load(obj->materials[0].map_Ke.name);
    ((nintendo::Model *)model)->spc.Load(obj->materials[0].map_Kt.name);

    // initialize gfx resources
    mesh->sampler = sg_make_sampler({
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .label = "model-sampler",
    });
    mesh->vertex_buffer = sg_make_buffer({
        .type = SG_BUFFERTYPE_VERTEXBUFFER,
        .data = {
            .ptr = mesh->vertices.data(),
            .size = mesh->vertices.size() * sizeof(float),
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