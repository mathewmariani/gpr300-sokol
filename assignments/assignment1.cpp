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
#include "batteries/gizmo.h"
#include "batteries/skybox.h"

// shaders
#include "batteries/shaders/no_post_process.h"
#include "batteries/shaders/chromatic_aberration.h"
#include "batteries/shaders/crt_post_process.h"
#include "batteries/shaders/blur_post_process.h"
#include "batteries/shaders/grayscale_post_process.h"
#include "batteries/shaders/inverse_post_process.h"

#include <array>
#include <string>
#include <vector>

static constexpr glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};

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
    batteries::blinnphong_t blinnphong;
    batteries::gizmo_t gizmo;
    batteries::skybox_t skybox;

    struct
    {
        sg_pass_action action;
        sg_pipeline pip;
        sg_bindings bind;
    } postprocess;

    int effect_index;

    batteries::camera_t camera;
    batteries::camera_controller_t camera_controller;

    batteries::ambient_t ambient;
    batteries::light_t light;

    struct
    {
        float ry;
        batteries::model_t suzanne;
        batteries::material_t material;
    } scene;
} state = {
    .effect_index = 0,
    .ambient = {
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    },
    .light = {
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
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

void create_postprocess_pass()
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

    auto postprocess_shader_desc = (sg_shader_desc){
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

    state.postprocess.action = (sg_pass_action){
        .colors[0].load_action = SG_LOADACTION_CLEAR,
        .depth.load_action = SG_LOADACTION_DONTCARE,
        .stencil.load_action = SG_LOADACTION_DONTCARE,
    };

    state.postprocess.pip = sg_make_pipeline({
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT2,
                [1].format = SG_VERTEXFORMAT_FLOAT2,
            },
        },
        .shader = sg_make_shader(postprocess_shader_desc),
        .label = "postprocess-pipeline",
    });

    // apply bindings
    state.postprocess.bind = (sg_bindings){
        .vertex_buffers[0] = sg_make_buffer({
            .data = SG_RANGE(quad_vertices),
            .label = "quad-vertices",
        }),
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
    batteries::create_blinnphong_pass(&state.blinnphong);
    batteries::create_gizmo_pass(&state.gizmo);
    batteries::create_skybox_pass(&state.skybox);

    load_suzanne();
    create_postprocess_pass();

    // apply bindings
    state.blinnphong.bind = (sg_bindings){
        .vertex_buffers[0] = state.scene.suzanne.mesh.vbuf,
    };
}

void draw_ui(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    if (ImGui::CollapsingHeader("Ambient"))
    {
        ImGui::SliderFloat("Intensity", &state.ambient.intensity, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color", &state.ambient.color[0]);
    }
    if (ImGui::CollapsingHeader("Light"))
    {
        ImGui::SliderFloat("Brightness", &state.light.brightness, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color", &state.light.color[0]);
    }
    if (ImGui::BeginCombo("Effect", post_processing_effects[state.effect_index].c_str()))
    {
        for (auto n = 0; n < post_processing_effects.size(); ++n)
        {
            auto is_selected = (post_processing_effects[state.effect_index] == post_processing_effects[n]);
            if (ImGui::Selectable(post_processing_effects[n].c_str(), is_selected))
            {
                state.effect_index = n;
                create_postprocess_pass();
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
    state.light.position = rym * light_orbit_radius;

    const auto view_proj = state.camera.projection() * state.camera.view();

    // initialize uniform data
    const batteries::vs_blinnphong_params_t vs_params = {
        .view_proj = view_proj,
        .model = state.scene.suzanne.transform.matrix(),
    };
    const batteries::fs_blinnphong_params_t fs_params = {
        .material = state.scene.material,
        .light = state.light,
        .ambient = state.ambient,
        .camera_position = state.camera.position,
    };
    const batteries::vs_gizmo_params_t vs_gizmo_params = {
        .view_proj = view_proj,
        .model = glm::translate(glm::mat4(1.0f), state.light.position),
    };
    const batteries::fs_gizmo_light_params_t fs_gizmo_light_params = {
        .light_color = state.light.color,
    };
    const batteries::vs_skybox_params_t vs_skybox_params = {
        .view_proj = state.camera.projection() * glm::mat4(glm::mat3(state.camera.view())),
    };

    sg_begin_pass(&state.framebuffer.pass);

    // blinn-phong pass
    sg_apply_pipeline(state.blinnphong.pip);
    sg_apply_bindings(&state.blinnphong.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_params));
    sg_draw(0, state.scene.suzanne.mesh.num_faces * 3, 1);

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

    // postprocess pass
    sg_begin_pass({.action = state.postprocess.action, .swapchain = sglue_swapchain()});
    sg_apply_pipeline(state.postprocess.pip);
    sg_apply_bindings(&state.postprocess.bind);
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