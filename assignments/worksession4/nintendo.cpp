#include "nintendo.h"

// fastobj
#include "fast_obj/fast_obj.h"

namespace nintendo
{
  static void fetch_callback(batteries::Model *model, fastObjMesh *obj)
  {
    batteries::Mesh *mesh = &model->mesh;

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

    // FIXME: load materials from lib
    ((nintendo::Model *)model)->albedo.Load(obj->materials[0].map_Kd.name);
    ((nintendo::Model *)model)->zatoon.Load("assets/windwaker/ZAToon.png");

    // initialize gfx resources
    mesh->bindings = (sg_bindings){
        .vertex_buffers[0] = sg_make_buffer({
            .data = {
                .ptr = mesh->vertices.data(),
                .size = mesh->vertices.size() * sizeof(float),
            },
            .label = "mesh-vertices",
        }),
        .fs = {
            .images = {
                [0] = ((nintendo::Model *)model)->albedo.image,
                [1] = ((nintendo::Model *)model)->zatoon.image,
            },
            .samplers[0] = sg_make_sampler({
                .min_filter = SG_FILTER_LINEAR,
                .mag_filter = SG_FILTER_LINEAR,
                .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
                .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
                .label = "model-sampler",
            }),
        },
    };
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