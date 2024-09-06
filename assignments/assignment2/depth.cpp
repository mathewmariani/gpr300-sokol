#include "depth.h"
#include "batteries/shaders/shadow_depth.h"

Depth::Depth()
{
    pass_action = (sg_pass_action){
        .colors[0].load_action = SG_LOADACTION_DONTCARE,
        .depth = {
            .load_action = SG_LOADACTION_CLEAR,
            .store_action = SG_STOREACTION_STORE,
            .clear_value = 1.0f,
        },
    };

    pip = sg_make_pipeline({
        .layout = {
            // need to provide vertex stride, because normal and texcoords components are skipped in shadow pass
            .buffers[0].stride = 8 * sizeof(float),
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT3,
            },
        },
        .shader = sg_make_shader({
            .vs = {
                .source = shadow_depth_vs,
                .uniform_blocks[0] = {
                    .layout = SG_UNIFORMLAYOUT_NATIVE,
                    .size = sizeof(vs_params_t),
                    .uniforms = {
                        [0] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                        [1] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                    },
                },
            },
            .fs = {
                .source = shadow_depth_fs,
            },
        }),
        .index_type = SG_INDEXTYPE_NONE,
        .face_winding = SG_FACEWINDING_CCW,
        .cull_mode = SG_CULLMODE_FRONT,
        .colors[0].pixel_format = SG_PIXELFORMAT_NONE,
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        .label = "depthbuffer-pipeline",
    });
}

void Depth::Render(const vs_params_t vs_params, batteries::model_t model)
{
    sg_apply_pipeline(pip);
    sg_apply_bindings(&model.mesh.bindings);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
    sg_draw(0, model.mesh.num_faces * 3, 1);
}