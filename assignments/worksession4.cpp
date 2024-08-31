#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Work Session 4 -- Toonshader
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"
#include "batteries/materials.h"
#include "batteries/lights.h"

// shaders
#include "batteries/shaders/toon_shadowing.h"

typedef struct
{
    glm::vec3 highlight;
    glm::vec3 shadow;
} palette_t;

typedef struct
{
    glm::mat4 view_proj;
    glm::mat4 model;
} vs_display_params_t;

typedef struct
{
    batteries::ambient_t ambient;
    palette_t palette;
} fs_display_params_t;

// application state
static struct
{
    uint8_t file_buffer[boilerplate::megabytes(5)];

    palette_t pal[4];

    struct
    {
        sg_pass_action pass_action;
        sg_pass pass;
        sg_pipeline pip;
        sg_bindings bind;
    } object_pass;

    struct
    {
        float ry;
        batteries::ambient_t ambient;
        palette_t palette;

        struct
        {
            batteries::model_t model;
            sg_image albedo;
            sg_image zatoon;
        } skull;
    } scene;

    batteries::camera_t camera;
    batteries::camera_controller_t camera_controller;
} state = {
    .pal = {
        {.highlight = glm::vec3(1.00f, 1.00f, 1.00f), .shadow = glm::vec3(0.60f, 0.54f, 0.52f)},
        {.highlight = glm::vec3(0.47f, 0.58f, 0.68f), .shadow = glm::vec3(0.32f, 0.39f, 0.57f)},
        {.highlight = glm::vec3(0.62f, 0.69f, 0.67f), .shadow = glm::vec3(0.50f, 0.55f, 0.50f)},
        {.highlight = glm::vec3(0.24f, 0.36f, 0.54f), .shadow = glm::vec3(0.25f, 0.31f, 0.31f)},
    },
    .scene = {
        .ry = 0.0f,
        .ambient = {
            .color = glm::vec3(0.25f, 0.45f, 0.65f),
            .intensity = 1.0f,
        },
        .palette = {
            .highlight = glm::vec3(1.00f, 1.00f, 1.00f),
            .shadow = glm::vec3(0.60f, 0.54f, 0.52f),
        },
    },
};

void load_skull(void)
{
    state.scene.skull.model.mesh.vbuf = sg_alloc_buffer();
    batteries::load_obj({
        .buffer_id = state.scene.skull.model.mesh.vbuf,
        .mesh = &state.scene.skull.model.mesh,
        .path = "assets/skull/skull.obj",
        .buffer = SG_RANGE(state.file_buffer),
    });
    state.scene.skull.albedo = sg_alloc_image();
    batteries::load_img({
        .image_id = state.scene.skull.albedo,
        .path = "assets/skull/skull.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
    state.scene.skull.zatoon = sg_alloc_image();
    batteries::load_img({
        .image_id = state.scene.skull.zatoon,
        .path = "assets/skull/ZAtoon.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
}

void create_object_pass(void)
{
    auto shader_desc = (sg_shader_desc){
        .vs = {
            .source = toon_shadowing_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(vs_display_params_t),
                .uniforms = {
                    [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                    [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                },
            },
        },
        .fs = {
            .source = toon_shadowing_fs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(fs_display_params_t),
                .uniforms = {
                    [0] = {.name = "ambient.intensity", .type = SG_UNIFORMTYPE_FLOAT},
                    [1] = {.name = "ambient.direction", .type = SG_UNIFORMTYPE_FLOAT3},
                    [2] = {.name = "ambient.color", .type = SG_UNIFORMTYPE_FLOAT3},
                    [3] = {.name = "palette.highlight", .type = SG_UNIFORMTYPE_FLOAT3},
                    [4] = {.name = "palette.shadow", .type = SG_UNIFORMTYPE_FLOAT3},
                },
            },
            .images = {
                [0] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
                [1] = {.used = true, .sample_type = SG_IMAGESAMPLETYPE_FLOAT},
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
                    .glsl_name = "material.zatoon",
                    .image_slot = 1,
                    .sampler_slot = 0,
                },
            },
        },
    };

    state.object_pass.pass_action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {state.scene.ambient.color.r, state.scene.ambient.color.g, state.scene.ambient.color.b, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        },
    };

    state.object_pass.pip = sg_make_pipeline({
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
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        .label = "object-pipeline",
    });

    auto smp = sg_make_sampler({
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .label = "object-sampler",
    });

    state.object_pass.bind = (sg_bindings){
        .vertex_buffers[0] = state.scene.skull.model.mesh.vbuf,
        .fs = {
            .images{
                [0] = state.scene.skull.albedo,
                [1] = state.scene.skull.zatoon,
            },
            .samplers[0] = smp,
        },
    };
}

void init(void)
{
    boilerplate::setup();

    load_skull();
    create_object_pass();
}

void draw_ui(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::Text("Presets");
    if (ImGui::Button("Sunny Day"))
    {
        state.scene.palette = state.pal[0];
    }
    if (ImGui::Button("Bright Night"))
    {
        state.scene.palette = state.pal[1];
    }
    if (ImGui::Button("Rainy Day"))
    {
        state.scene.palette = state.pal[2];
    }
    if (ImGui::Button("Rainy Night"))
    {
        state.scene.palette = state.pal[3];
    }

    ImGui::Text("Palette");
    ImGui::ColorEdit3("Highlight", &state.scene.palette.highlight[0]);
    ImGui::ColorEdit3("Shadow", &state.scene.palette.shadow[0]);

    ImGui::End();
}

void frame(void)
{
    boilerplate::frame();
    draw_ui();

    const auto t = (float)sapp_frame_duration();
    state.camera_controller.update(&state.camera, t);

    // math required by the scene
    const auto camera_view_proj = state.camera.projection() * state.camera.view();
    const auto light_pos = glm::vec4(10.0f, 10.0f, 0.0f, 0.0f);
    state.scene.ambient.direction = glm::normalize(light_pos);

    // sugar: rotate and scale
    state.scene.skull.model.transform.rotation = glm::rotate(state.scene.skull.model.transform.rotation, t, glm::vec3(0.0, 1.0, 0.0));
    state.scene.skull.model.transform.scale = glm::vec3(0.05f);

    // initialize uniform data
    const vs_display_params_t vs_params = {
        .view_proj = camera_view_proj,
        .model = state.scene.skull.model.transform.matrix(),
    };
    const fs_display_params_t fs_params = {
        .ambient = state.scene.ambient,
        .palette = state.scene.palette,
    };

    // graphics pass
    sg_begin_pass({.action = state.object_pass.pass_action, .swapchain = sglue_swapchain()});
    sg_apply_pipeline(state.object_pass.pip);
    sg_apply_bindings(&state.object_pass.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
    sg_draw(0, state.scene.skull.model.mesh.num_faces * 3, 1);

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