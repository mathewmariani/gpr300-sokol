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

#include "batteries/framebuffer.h"
#include "batteries/gizmo.h"
#include "batteries/skybox.h"

// shaders
#include "batteries/shaders/pbr_pass.h"

static constexpr glm::vec4 light_orbit_radius = {2.0f, 2.0f, -2.0f, 1.0f};

typedef struct
{
    glm::mat4 view_proj;
    glm::mat4 model;
} vs_pbr_params_t;

typedef struct
{
    batteries::light_t light;
    glm::vec3 camera_position;
} fs_pbr_params_t;

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
    uint8_t file_buffer[boilerplate::megabytes(5)];

    batteries::framebuffer_t framebuffer;
    batteries::gizmo_t gizmo;
    batteries::skybox_t skybox;

    batteries::camera_t camera;
    batteries::camera_controller_t camera_controller;

    float ry;

    struct
    {
        sg_pass_action action;
        sg_pipeline pip;
        sg_bindings bind;
    } pbr;

    struct
    {
        batteries::light_t light;
        batteries::model_t togezoshell;
        pbr_material_t material;
    } scene;
} state = {
    .ry = 0.0f,
    .scene = {
        .light = {
            .brightness = 1.0f,
            .color = {1.0f, 1.0f, 1.0f},
        },
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
                },
            },
        },
        .fs = {
            .source = pbr_pass_fs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(fs_pbr_params_t),
                .uniforms = {
                    [0] = {.name = "light.brightness", .type = SG_UNIFORMTYPE_FLOAT},
                    [1] = {.name = "light.color", .type = SG_UNIFORMTYPE_FLOAT3},
                    [2] = {.name = "light.position", .type = SG_UNIFORMTYPE_FLOAT3},
                    [3] = {.name = "camera_position", .type = SG_UNIFORMTYPE_FLOAT3},
                },
            },
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

    state.pbr.action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
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
        .colors = {
            [0].pixel_format = SG_PIXELFORMAT_RGBA8,
        },
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        .label = "pbr-pipeline",
    });
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

    // transform
    state.scene.togezoshell.transform.rotation = glm::rotate(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    state.scene.togezoshell.transform.scale = glm::vec3(0.5f);

    state.scene.material = {
        .col = sg_alloc_image(),
        .mtl = sg_alloc_image(),
        .rgh = sg_alloc_image(),
        .ao = sg_alloc_image(),
        .spc = sg_alloc_image(),
    };

    state.scene.togezoshell.mesh.bindings = (sg_bindings){
        .vertex_buffers[0] = state.scene.togezoshell.mesh.vbuf,
        .fs = {
            .images = {
                [0] = state.scene.material.col,
                [1] = state.scene.material.mtl,
                [2] = state.scene.material.rgh,
                [3] = state.scene.material.ao,
                [4] = state.scene.material.spc,
            },
            .samplers[0] = sg_make_sampler({
                .min_filter = SG_FILTER_LINEAR,
                .mag_filter = SG_FILTER_LINEAR,
                .wrap_u = SG_WRAP_REPEAT,
                .wrap_v = SG_WRAP_REPEAT,
                .label = "model-sampler",
            }),
        },
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
    const auto width = sapp_width();
    const auto height = sapp_height();

    boilerplate::setup();
    batteries::create_framebuffer(&state.framebuffer, width, height);
    batteries::create_gizmo_pass(&state.gizmo);
    batteries::create_skybox_pass(&state.skybox);

    load_togezoshell();
    create_pbr_pass();

    // apply bindings
    state.pbr.bind = state.scene.togezoshell.mesh.bindings;
}

void draw_ui(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void frame(void)
{
    boilerplate::frame();

    const auto t = (float)sapp_frame_duration();
    state.camera_controller.update(&state.camera, t);
    state.ry += 0.2f * t;

    // math required by the scene
    const auto camera_view_proj = state.camera.projection() * state.camera.view();

    // sugar: rotate light
    const auto rym = glm::rotate(state.ry, glm::vec3(0.0f, 1.0f, 0.0f));
    state.scene.light.position = rym * light_orbit_radius;

    // initialize uniform data
    const vs_pbr_params_t vs_params = {
        .view_proj = camera_view_proj,
        .model = state.scene.togezoshell.transform.matrix(),
    };
    const fs_pbr_params_t fs_params = {
        .light = state.scene.light,
        .camera_position = state.camera.position,
    };
    const batteries::vs_gizmo_params_t vs_gizmo_params = {
        .view_proj = camera_view_proj,
        .model = glm::translate(glm::mat4(1.0f), state.scene.light.position),
    };
    const batteries::fs_gizmo_light_params_t fs_gizmo_light_params = {
        .light_color = state.scene.light.color,
    };
    const batteries::vs_skybox_params_t vs_skybox_params = {
        .view_proj = state.camera.projection() * glm::mat4(glm::mat3(state.camera.view())),
    };

    sg_begin_pass(&state.framebuffer.pass);

    // physically based rendering pass
    sg_apply_pipeline(state.pbr.pip);
    sg_apply_bindings(&state.pbr.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
    sg_draw(0, state.scene.togezoshell.mesh.num_faces * 3, 1);

    // render light sources
    sg_apply_pipeline(state.gizmo.pip);
    sg_apply_bindings(&state.gizmo.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_gizmo_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_gizmo_light_params));
    sg_draw(state.gizmo.sphere.base_element, state.gizmo.sphere.num_elements, 1);

    // render skybox
    sg_apply_pipeline(state.skybox.pip);
    sg_apply_bindings(&state.skybox.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_skybox_params));
    sg_draw(0, 36, 1);

    sg_end_pass();

    // render framebuffer
    sg_begin_pass({.action = state.framebuffer.action, .swapchain = sglue_swapchain()});
    sg_apply_pipeline(state.framebuffer.pip);
    sg_apply_bindings(&state.framebuffer.bind);
    sg_draw(0, 6, 1);

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