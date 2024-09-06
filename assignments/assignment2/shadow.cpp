#include "shadow.h"
#include "batteries/shaders/shadow_map.h"

Shadow::Shadow()
{
  auto shader_desc = (sg_shader_desc){
      .vs = {
          .source = shadow_map_vs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(vs_params_t),
              .uniforms = {
                  [0] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                  [1] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                  [2] = {.name = "light_view_proj", .type = SG_UNIFORMTYPE_MAT4},
              },
          },
      },
      .fs = {
          .source = shadow_map_fs,
          .uniform_blocks[0] = {
              .layout = SG_UNIFORMLAYOUT_NATIVE,
              .size = sizeof(fs_params_t),
              .uniforms = {
                  [0] = {.name = "light_pos", .type = SG_UNIFORMTYPE_FLOAT3},
                  [1] = {.name = "eye_pos", .type = SG_UNIFORMTYPE_FLOAT3},
                  [2] = {.name = "material.ambient", .type = SG_UNIFORMTYPE_FLOAT3},
                  [3] = {.name = "material.diffuse", .type = SG_UNIFORMTYPE_FLOAT3},
                  [4] = {.name = "material.specular", .type = SG_UNIFORMTYPE_FLOAT3},
                  [5] = {.name = "material.shininess", .type = SG_UNIFORMTYPE_FLOAT},
                  [6] = {.name = "ambient.intensity", .type = SG_UNIFORMTYPE_FLOAT},
                  [7] = {.name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                  [8] = {.name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
              },
          },
          .images[0] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_DEPTH},
          .samplers[0] = {.used = true, .sampler_type = SG_SAMPLERTYPE_COMPARISON},
          .image_sampler_pairs[0] = {
              .glsl_name = "shadow_map",
              .image_slot = 0,
              .sampler_slot = 0,
              .used = true,
          },
      },
  };
  action = (sg_pass_action){
      .colors[0] = {
          .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
          .load_action = SG_LOADACTION_CLEAR,
      },
  };
  pip = sg_make_pipeline({
      .layout = {
          .attrs = {
              [0].format = SG_VERTEXFORMAT_FLOAT3,
              [1].format = SG_VERTEXFORMAT_FLOAT3,
              [2].format = SG_VERTEXFORMAT_FLOAT2,
          },
      },
      .shader = sg_make_shader(shader_desc),
      .index_type = SG_INDEXTYPE_NONE,
      .face_winding = SG_FACEWINDING_CCW,
      .cull_mode = SG_CULLMODE_BACK,
      .depth = {
          .pixel_format = SG_PIXELFORMAT_DEPTH,
          .compare = SG_COMPAREFUNC_LESS_EQUAL,
          .write_enabled = true,
      },
      .label = "shadow-pipeline",
  });

  bind = (sg_bindings){
      // .fs = {
      //     .images[0] = state.depth.img,
      //     .samplers[0] = state.depth.smp,
      // },
  };
}

void Shadow::Render(const vs_params_t vs_params, const fs_params_t fs_params, batteries::model_t model)
{
  sg_apply_pipeline(pip);
  sg_apply_bindings(&model.mesh.bindings);
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
  sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
  sg_draw(0, model.mesh.num_faces * 3, 1);
}