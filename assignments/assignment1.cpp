#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Assignment1 -- Post Processing
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"

#include "batteries/framebuffer.h"
#include "batteries/blinnphong.h"

// shaders
#include "shaders/blinn_phong.h"
#include "shaders/no_post_process.h"
#include "shaders/chromatic_aberration.h"
#include "shaders/crt_post_process.h"
#include "shaders/blur_post_process.h"
#include "shaders/grayscale_post_process.h"
#include "shaders/inverse_post_process.h"
#include "shaders/shapes.h"

#include <array>
#include <string>
#include <vector>

typedef struct
{
    glm::mat4 view_proj;
    glm::mat4 model;
} vs_gizmo_params_t;

typedef struct
{
    glm::vec3 light_color;
} fs_gizmo_light_params_t;

std::vector<std::string> post_processing_effects = {
    "None",
    "Grayscale",
    "Kernel Blur",
    "Inverse",
    "Chromatic Aberration",
    "CRT",
};

// application state
static struct
{
    uint8_t file_buffer[boilerplate::megabytes(4)];

    batteries::framebuffer_t framebuffer;

    struct
    {
        sg_pass_action pass_action;
        sg_pipeline pip;
        sg_bindings bind;
    } blinnphong;

    struct
    {
        sg_pass_action pass_action;
        sg_pipeline pip;
        sg_bindings bind;
        sshape_element_range_t sphere;
    } gizmo;

    struct
    {
        sg_pass_action pass_action;
        sg_pipeline pip;
        sg_bindings bind;
    } display;

    int effect_index;

    batteries::camera_t camera;
    batteries::camera_controller_t camera_controller;
    batteries::light_t light;

    struct
    {
        float ry;
        batteries::model_t suzanne;
        batteries::material_t material;
    } scene;
} state = {
    .effect_index = 0,
    .light = {
        .brightness = 1.0f,
        .color = glm::vec3(0.25f, 0.45f, 0.65f),
    },
    .scene = {
        .ry = 0.0f,
        .material = {
            .ambient = {0.5f, 0.5f, 0.5f},
            .diffuse = {0.5f, 0.5f, 0.5f},
            .specular = {0.5f, 0.5f, 0.5f},
            .shininess = 128.0f,
        },
    },
};

void load_suzanne(void)
{
    state.scene.suzanne.mesh.vbuf = sg_alloc_buffer();
    batteries::load_obj({
        .buffer_id = state.scene.suzanne.mesh.vbuf,
        .mesh = &state.scene.suzanne.mesh,
        .path = "assets/suzanne.obj",
        .buffer = SG_RANGE(state.file_buffer),
    });
}

void create_blinnphong_pass(void)
{
    auto shader_desc = (sg_shader_desc){
        .vs = {
            .source = blinn_phong_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(batteries::vs_blinnphong_params_t),
                .uniforms = {
                    [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                    [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                },
            },
        },
        .fs = {
            .source = blinn_phong_fs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(batteries::fs_blinnphong_params_t),
                .uniforms = {
                    [0] = {.name = "material.ambient", .type = SG_UNIFORMTYPE_FLOAT3},
                    [1] = {.name = "material.diffuse", .type = SG_UNIFORMTYPE_FLOAT3},
                    [2] = {.name = "material.specular", .type = SG_UNIFORMTYPE_FLOAT3},
                    [3] = {.name = "material.shininess", .type = SG_UNIFORMTYPE_FLOAT},
                    [4] = {.name = "light.brightness", .type = SG_UNIFORMTYPE_FLOAT},
                    [5] = {.name = "light.color", .type = SG_UNIFORMTYPE_FLOAT3},
                    [6] = {.name = "light.position", .type = SG_UNIFORMTYPE_FLOAT3},
                    [7] = {.name = "camera_position", .type = SG_UNIFORMTYPE_FLOAT3},
                },
            },
        },
    };

    state.blinnphong.pass_action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        },
    };

    state.blinnphong.pip = sg_make_pipeline({
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
        .colors = {
            [0].pixel_format = SG_PIXELFORMAT_RGBA8,
        },
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        .label = "blinnphong-pipeline",
    });

    state.blinnphong.bind = (sg_bindings){
        .vertex_buffers[0] = state.scene.suzanne.mesh.vbuf,
    };
}

void create_gizmo_pass(void)
{
    auto shader_desc = (sg_shader_desc){
        .vs = {
            .source = shapes_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(vs_gizmo_params_t),
                .uniforms = {
                    [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                    [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                },
            },
        },
        .fs = {
            .source = shapes_fs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(fs_gizmo_light_params_t),
                .uniforms = {
                    [0] = {.name = "light_color", .type = SG_UNIFORMTYPE_FLOAT3},
                },
            },
        },
    };

    state.gizmo.pass_action = (sg_pass_action){
        .colors[0].load_action = SG_LOADACTION_LOAD,
        .depth.load_action = SG_LOADACTION_LOAD,
    };

    state.gizmo.pip = sg_make_pipeline({
        .shader = sg_make_shader(shader_desc),
        .layout = {
            .buffers[0] = sshape_vertex_buffer_layout_state(),
            .attrs = {
                [0] = sshape_position_vertex_attr_state(),
                [1] = sshape_normal_vertex_attr_state(),
                [2] = sshape_texcoord_vertex_attr_state(),
                [3] = sshape_color_vertex_attr_state(),
            },
        },
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_NONE,
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        .label = "gizmo-pipeline",
    });

    // generate shape geometries
    sshape_vertex_t vertices[30] = {0}; // (slices + 1) * (stacks + 1);
    uint16_t indices[90] = {0};         // ((2 * slices * stacks) - (2 * slices)) * 3;
    sshape_buffer_t buf = {
        .vertices.buffer = SSHAPE_RANGE(vertices),
        .indices.buffer = SSHAPE_RANGE(indices),
    };
    const sshape_sphere_t sphere = {
        .radius = 0.125f,
        .slices = 5,
        .stacks = 4,
    };
    buf = sshape_build_sphere(&buf, &sphere);
    assert(buf.valid);

    // one vertex/index-buffer-pair for all shapes
    state.gizmo.sphere = sshape_element_range(&buf);
    const sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&buf);
    const sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&buf);
    state.gizmo.bind = (sg_bindings){
        .vertex_buffers[0] = sg_make_buffer(&vbuf_desc),
        .index_buffer = sg_make_buffer(&ibuf_desc),
    };
}

void create_display_pass()
{
    static std::vector<std::array<std::string, 2>> shader_stage = {
        {no_post_process_vs, no_post_process_fs},
        {grayscale_post_process_vs, grayscale_post_process_fs},
        {blur_post_process_vs, blur_post_process_fs},
        {inverse_post_process_vs, inverse_post_process_fs},
        {chromatic_aberration_vs, chromatic_aberration_fs},
        {crt_post_process_vs, crt_post_process_fs},
    };

    // clang-format off
  float quad_vertices[] = {
     -1.0f, 1.0f, 0.0f, 1.0f,
     -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 1.0f, 0.0f,

     -1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 1.0f, 1.0f
  };
    // clang-format on

    auto quad_buffer = sg_make_buffer({
        .data = SG_RANGE(quad_vertices),
        .label = "quad-vertices",
    });

    auto display_shader_desc = (sg_shader_desc){
        .vs = {
            .source = shader_stage[state.effect_index][0].c_str(),
        },
        .fs = {
            .source = shader_stage[state.effect_index][1].c_str(),
            .images[0].used = true,
            .samplers[0].used = true,
            .image_sampler_pairs[0] = {
                .glsl_name = "screen",
                .image_slot = 0,
                .sampler_slot = 0,
                .used = true,
            },
        },
    };

    state.display.pass_action = (sg_pass_action){
        .colors[0].load_action = SG_LOADACTION_CLEAR,
        .depth.load_action = SG_LOADACTION_DONTCARE,
        .stencil.load_action = SG_LOADACTION_DONTCARE,
    };

    state.display.pip = sg_make_pipeline({
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT2,
                [1].format = SG_VERTEXFORMAT_FLOAT2,
            },
        },
        .shader = sg_make_shader(display_shader_desc),
        .label = "display-pipeline",
    });

    // apply bindings
    state.display.bind = (sg_bindings){
        .vertex_buffers[0] = quad_buffer,
        .fs = {
            .images[0] = state.framebuffer.color,
            .samplers[0] = state.framebuffer.sampler,
        },
    };
}

void init(void)
{
    const auto width = sapp_width();
    const auto height = sapp_height();
    boilerplate::setup();
    batteries::create_framebuffer(&state.framebuffer, width, height);

    load_suzanne();
    create_blinnphong_pass();
    create_gizmo_pass();
    create_display_pass();
}

void draw_ui(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    if (ImGui::BeginCombo("Effect", post_processing_effects[state.effect_index].c_str()))
    {
        for (auto n = 0; n < post_processing_effects.size(); ++n)
        {
            auto is_selected = (post_processing_effects[state.effect_index] == post_processing_effects[n]);
            if (ImGui::Selectable(post_processing_effects[n].c_str(), is_selected))
            {
                state.effect_index = n;
                create_display_pass();
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::End();
}

void frame(void)
{
    boilerplate::frame();
    draw_ui();

    const auto t = (float)sapp_frame_duration();
    state.camera_controller.update(&state.camera, t);
    state.scene.ry += 0.2f * t;

    // sugar: rotate light
    const auto rym = glm::rotate(state.scene.ry, glm::vec3(0.0f, 1.0f, 0.0f));
    state.light.position = rym * glm::vec4(5.0f, 0.0f, 5.0f, 1.0f);

    const auto view_proj = state.camera.projection() * state.camera.view();

    // initialize uniform data
    const batteries::vs_blinnphong_params_t vs_params = {
        .view_proj = view_proj,
        .model = state.scene.suzanne.transform.matrix(),
    };
    const batteries::fs_blinnphong_params_t fs_params = {
        .material = state.scene.material,
        .light = state.light,
        .camera_position = state.camera.position,
    };
    const vs_gizmo_params_t vs_gizmo = {
        .view_proj = view_proj,
        .model = glm::translate(glm::mat4(1.0f), state.light.position),
    };
    const fs_gizmo_light_params_t fs_gizmo_light = {
        .light_color = state.light.color,
    };

    // blinn-phong pass
    sg_begin_pass({.action = state.blinnphong.pass_action, .attachments = state.framebuffer.attachments});
    sg_apply_pipeline(state.blinnphong.pip);
    sg_apply_bindings(&state.blinnphong.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
    sg_draw(0, state.scene.suzanne.mesh.num_faces * 3, 1);
    sg_end_pass();

    // render light sources.
    sg_begin_pass({.action = state.gizmo.pass_action, .attachments = state.framebuffer.attachments});
    sg_apply_pipeline(state.gizmo.pip);
    sg_apply_bindings(&state.gizmo.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_gizmo));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_gizmo_light));
    sg_draw(state.gizmo.sphere.base_element, state.gizmo.sphere.num_elements, 1);
    sg_end_pass();

    // display pass
    sg_begin_pass({.action = state.display.pass_action, .swapchain = sglue_swapchain()});
    sg_apply_pipeline(state.display.pip);
    sg_apply_bindings(&state.display.bind);
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