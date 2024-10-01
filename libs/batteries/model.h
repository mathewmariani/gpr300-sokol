#pragma once

// batteries
#include "transform.h"
#include "vertex.h"
#include "mesh.h"

// sokol
#include "sokol/sokol_gfx.h"

// stl
#include <vector>

namespace batteries
{
  struct model_t
  {
    mesh_t mesh;
    transform_t transform;
    std::vector<sg_image> textures;

    void Render(void)
    {
      if (mesh.obj->group_count > 0)
      {
        fastObjGroup *group = nullptr;
        for (auto i = 0; i < mesh.obj->group_count; i++)
        {
          mesh.bindings.fs.images[0] = textures[i];
          sg_apply_bindings(&mesh.bindings);

          group = mesh.obj->groups + i;
          sg_draw(group->index_offset, group->face_count * 3, 1);
        }
      }
      else
      {
        sg_apply_bindings(&mesh.bindings);
        sg_draw(0, mesh.num_faces * 3, 1);
      }
    };
  };

  struct Model
  {
    Mesh mesh;
    transform_t transform;

    void Render(void)
    {
      if (!mesh.loaded)
      {
        return;
      }

      sg_apply_bindings(&mesh.bindings);
      sg_draw(0, mesh.num_faces * 3, 1);
    };
  };
}