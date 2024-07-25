#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Assignment0 -- Physically Based Rendering
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"
#include "batteries/materials.h"
#include "batteries/lights.h"

// shaders
#include "shaders/pbr_pass.h"

typedef struct
{
    glm::mat4 view_proj;
    glm::mat4 model;
    glm::vec3 camera_pos;
    glm::vec3 light_pos;
    glm::vec3 ambient;
} vs_pbr_params_t;

typedef struct
{
    sg_image col;
    sg_image mtl;
    sg_image rgh;
    sg_image ao;
    sg_image spc;
} pbr_material_t;

// application state
static struct
{
    struct
    {
        sg_pass_action pass_action;
        sg_pass pass;
        sg_pipeline pip;
        sg_bindings bind;
    } pbr;

    batteries::camera_t camera;
    batteries::camera_controller_t camera_controller;

    struct
    {
        float ry;
        glm::vec3 ambient_light;
        batteries::model_t togezoshell;
        pbr_material_t material;
    } scene;

    uint8_t file_buffer[boilerplate::megabytes(5)];
} state = {
    .scene = {
        .ry = 0.0f,
        .ambient_light = glm::vec3(0.25f, 0.45f, 0.65f),
    },
};

void create_pbr_pass(void)
{
    auto shader_desc = (sg_shader_desc){
        .vs = {
            .source = pbr_pass_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(vs_pbr_params_t),
                .uniforms = {
                    [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                    [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                    [2] = {.name = "cameraPos", .type = SG_UNIFORMTYPE_FLOAT3},
                    [3] = {.name = "lightPos", .type = SG_UNIFORMTYPE_FLOAT3},
                    [4] = {.name = "ambient", .type = SG_UNIFORMTYPE_FLOAT3},
                },
            },
        },
        .fs = {
            .source = pbr_pass_fs,
            .images = {
                [0] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                [1] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                [2] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                [3] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                [4] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
            },
            .samplers = {
                [0] = {.used = true, .sampler_type = SG_SAMPLERTYPE_FILTERING},
            },
            .image_sampler_pairs = {
                [0] = {
                    .used = true,
                    .glsl_name = "material.albedo",
                    .image_slot = 0,
                    .sampler_slot = 0,
                },
                [1] = {
                    .used = true,
                    .glsl_name = "material.metallic",
                    .image_slot = 1,
                    .sampler_slot = 0,
                },
                [2] = {
                    .used = true,
                    .glsl_name = "material.roughness",
                    .image_slot = 2,
                    .sampler_slot = 0,
                },
                [3] = {
                    .used = true,
                    .glsl_name = "material.occlusion",
                    .image_slot = 3,
                    .sampler_slot = 0,
                },
                [4] = {
                    .used = true,
                    .glsl_name = "material.specular",
                    .image_slot = 4,
                    .sampler_slot = 0,
                },
            },

        },
    };

    state.pbr.pass_action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {state.scene.ambient_light.r, state.scene.ambient_light.g, state.scene.ambient_light.b, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        }};

    state.pbr.pip = sg_make_pipeline({
        .layout = {
            .attrs = {
                // position, normal, texcoords
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
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        .label = "pbr-pipeline",
    });

    auto vbuf = sg_alloc_buffer();
    state.pbr.bind = (sg_bindings){
        .vertex_buffers[0] = state.scene.togezoshell.mesh.vbuf,
    };

    sg_sampler smp = sg_make_sampler({
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .wrap_u = SG_WRAP_REPEAT,
        .wrap_v = SG_WRAP_REPEAT,
        .label = "model-sampler",
    });

    state.pbr.bind.fs = {
        .images = {
            [0] = state.scene.material.col,
            [1] = state.scene.material.mtl,
            [2] = state.scene.material.rgh,
            [3] = state.scene.material.ao,
            [4] = state.scene.material.spc,
        },
        .samplers[0] = smp,
    };
}

void load_togezoshell(void)
{
    state.scene.togezoshell.mesh.vbuf = sg_alloc_buffer();
    batteries::load_obj({
        .buffer_id = state.scene.togezoshell.mesh.vbuf,
        .mesh = &state.scene.togezoshell.mesh,
        .path = "assets/objects/togezoshell/togezoshell.obj",
        .buffer = SG_RANGE(state.file_buffer),
    });

    state.scene.material = {
        .col = sg_alloc_image(),
        .mtl = sg_alloc_image(),
        .rgh = sg_alloc_image(),
        .ao = sg_alloc_image(),
        .spc = sg_alloc_image(),
    };

    batteries::load_img({
        .image_id = state.scene.material.ao,
        .path = "assets/materials/togezoshell/togezoshell_ao.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
    batteries::load_img({
        .image_id = state.scene.material.col,
        .path = "assets/materials/togezoshell/togezoshell_col.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
    batteries::load_img({
        .image_id = state.scene.material.mtl,
        .path = "assets/materials/togezoshell/togezoshell_mtl.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
    batteries::load_img({
        .image_id = state.scene.material.rgh,
        .path = "assets/materials/togezoshell/togezoshell_rgh.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
    batteries::load_img({
        .image_id = state.scene.material.spc,
        .path = "assets/materials/togezoshell/togezoshell_spc.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
}

void init(void)
{
    boilerplate::setup();
    load_togezoshell();
    create_pbr_pass();
}

void draw_ui(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    if (ImGui::ColorEdit3("Ambient Light", &state.scene.ambient_light[0]))
    {
        state.pbr.pass_action.colors[0].clear_value = {state.scene.ambient_light.r, state.scene.ambient_light.g, state.scene.ambient_light.b, 1.0f};
    }
    ImGui::End();
}

void frame(void)
{
    boilerplate::frame();

    const auto t = (float)sapp_frame_duration();
    state.camera_controller.update(&state.camera, t);
    state.scene.ry += 0.2f * t;

    // sugar: rotate suzzane
    state.scene.togezoshell.transform.rotation = glm::rotate(state.scene.togezoshell.transform.rotation, t, glm::vec3(0.0, 1.0, 0.0));
    state.scene.togezoshell.transform.scale = glm::vec3(0.5f);

    // sugar: rotate light
    const glm::mat4 rym = glm::rotate(state.scene.ry, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 light_pos = rym * glm::vec4(10.0f, 0.0f, -10.0f, 1.0f);

    // initialize uniform data
    const vs_pbr_params_t vs_params = {
        .view_proj = state.camera.projection() * state.camera.view(),
        .model = state.scene.togezoshell.transform.matrix(),
        .camera_pos = state.camera.position,
        .light_pos = light_pos,
        .ambient = state.scene.ambient_light,
    };

    // pbr pass
    sg_begin_pass({.action = state.pbr.pass_action, .swapchain = sglue_swapchain()});
    sg_apply_pipeline(state.pbr.pip);
    sg_apply_bindings(&state.pbr.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
    sg_draw(0, state.scene.togezoshell.mesh.num_faces * 3, 1);

    // draw ui
    simgui_render();

    sg_end_pass();
    sg_commit();
}

void event(const sapp_event *event)
{
    boilerplate::event(event);
    state.camera_controller.event(event);
}

void cleanup(void)
{
    boilerplate::shutdown();
}