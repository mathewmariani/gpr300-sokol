#include "assignment3.h"

// fastobj
#include "fast_obj/fast_obj.h"

namespace assignment3
{
  static void fetch_callback(batteries::Model *model, fastObjMesh *obj)
  {
    batteries::Mesh *mesh = &model->mesh;
    model->loaded = true;

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

    // initialize gfx resources
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