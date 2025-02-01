// batteries
#include "model.h"

// fastobj
#include "fast_obj/fast_obj.h"

namespace batteries
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

    // initialize gfx resources
    // mesh->vertex_buffer = sg_make_buffer({
    //     .type = SG_BUFFERTYPE_VERTEXBUFFER,
    //     .data = {
    //         .ptr = mesh->vertices.data(),
    //         .size = mesh->vertices.size() * sizeof(vertex_t),
    //     },
    //     .label = "mesh-vertices",
    // });
    // mesh->index_buffer = sg_make_buffer({
    //     .type = SG_BUFFERTYPE_INDEXBUFFER,
    //     .data = {
    //         .ptr = mesh->indices.data(),
    //         .size = mesh->indices.size() * sizeof(uint16_t),
    //     },
    //     .label = "mesh-indices",
    // });
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