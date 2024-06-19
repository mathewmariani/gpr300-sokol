#define BATTERIES_IMPL
#include "batteries.h"

//
// Assignment2 -- Shadow Mapping
//

// shaders
#include "shaders/shadow_depth.h"
#include "shaders/shadow_map.h"

#include <string>

typedef struct
{
    glm::mat4 model;
    glm::mat4 view_proj;
} vs_shadow_params_t;

typedef struct
{
    glm::mat4 model;
    glm::mat4 view_proj;
    glm::mat4 light_view_proj;
} vs_display_params_t;

typedef struct
{
    glm::vec3 light_pos;
    glm::vec3 eye_pos;
} fs_display_params_t;

// application state
static struct
{
    struct
    {
        sg_pass_action pass_action;
        sg_pipeline pip;
        sg_bindings bind;
    } display;

    struct
    {
        sg_pass_action pass_action;
        sg_attachments attachments;
        sg_pipeline pip;
        sg_bindings bind;
        sg_image map;
        sg_sampler sampler;
    } shadow;

    struct
    {
        simgui_image_t shadow_map;
    } ui;

    uint8_t file_buffer[batteries::megabytes(4)];

    batteries::model_t suzanne;
    sg_buffer plane_vbuf;
    sg_bindings plane_bind;
    float ry;
    glm::vec3 ambient_light;

} state = {
    .ambient_light = glm::vec3(0.25f, 0.45f, 0.65f),
    .ry = 0.0f,
};

void load_suzanne(void)
{
    state.suzanne.mesh.vbuf = sg_alloc_buffer();
    batteries::assets::load_obj({
        .buffer_id = state.suzanne.mesh.vbuf,
        .mesh = &state.suzanne.mesh,
        .path = "assets/suzanne.obj",
        .buffer = SG_RANGE(state.file_buffer),
    });
}

void create_shadow_pass(void)
{
    // create a texture with only a depth attachment
    state.shadow.map = sg_make_image((sg_image_desc){
        .render_target = true,
        .width = 1024,
        .height = 1024,
        .sample_count = 1,
        .pixel_format = SG_PIXELFORMAT_DEPTH,
        .label = "depth-image",
    });

    // create an image sampler
    state.shadow.sampler = sg_make_sampler((sg_sampler_desc){
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .compare = SG_COMPAREFUNC_LESS,
        .label = "shadow-sampler",
    });

    state.shadow.attachments = sg_make_attachments((sg_attachments_desc){
        .depth_stencil.image = state.shadow.map,
        .label = "shadow-pass",
    });

    auto shadow_shader_desc = (sg_shader_desc){
        .vs = {
            .source = shadow_depth_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(vs_shadow_params_t),
                .uniforms = {
                    [0] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                    [1] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                },
            },
        },
        .fs = {
            .source = shadow_depth_fs,
        },
    };

    state.shadow.pass_action = (sg_pass_action){
        .depth = {
            .load_action = SG_LOADACTION_CLEAR,
            .store_action = SG_STOREACTION_STORE,
            .clear_value = 1.0f,
        },
    };

    state.shadow.pip = sg_make_pipeline((sg_pipeline_desc){
        .layout = {
            // need to provide vertex stride, because normal and texcoords components are skipped in shadow pass
            .buffers[0].stride = 8 * sizeof(float),
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT3,
            },
        },
        .shader = sg_make_shader(shadow_shader_desc),
        .index_type = SG_INDEXTYPE_NONE,
        .face_winding = SG_FACEWINDING_CCW,
        .cull_mode = SG_CULLMODE_FRONT,
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        .colors[0].pixel_format = SG_PIXELFORMAT_NONE,
        .label = "shadow-pipeline",
    });

    state.shadow.bind = (sg_bindings){
        .vertex_buffers[0] = state.suzanne.mesh.vbuf,
    };
}

void create_display_pass(void)
{
    state.display.pass_action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {state.ambient_light.r, state.ambient_light.g, state.ambient_light.b, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        },
    };
    auto shader_desc = (sg_shader_desc){
        .vs = {
            .source = shadow_map_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(vs_display_params_t),
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
                .size = sizeof(fs_display_params_t),
                .uniforms = {
                    [0] = {.name = "light_pos", .type = SG_UNIFORMTYPE_FLOAT3},
                    [1] = {.name = "eye_pos", .type = SG_UNIFORMTYPE_FLOAT3},
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
    state.display.pip = sg_make_pipeline({
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
        .label = "display-pipeline",
    });

    state.display.bind = (sg_bindings){
        .vertex_buffers[0] = state.suzanne.mesh.vbuf,
        .fs = {
            .images[0] = state.shadow.map,
            .samplers[0] = state.shadow.sampler,
        },
    };
}

void init(void)
{
    batteries::setup();

    load_suzanne();
    create_shadow_pass();
    create_display_pass();

    // create an sokol-imgui wrapper for the shadow map
    auto ui_smp = sg_make_sampler((sg_sampler_desc){
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .label = "ui-sampler",
    });
    state.ui.shadow_map = simgui_make_image((simgui_image_desc_t){
        .image = state.shadow.map,
        .sampler = ui_smp,
    });

    // clang-format off
    const float plane_vertices[] = {
        // position, normal, texcoord:
        5.0f,  -1.5f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -5.0f,  -1.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -5.0f,  -1.5f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        5.0f,  -1.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -5.0f,  -1.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        5.0f,  -1.5f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    };
    // clang-format on

    state.plane_vbuf = sg_make_buffer((sg_buffer_desc){
        .data = SG_RANGE(plane_vertices),
        .label = "plane-vertices",
    });

    state.plane_bind = (sg_bindings){
        .vertex_buffers[0] = state.plane_vbuf,
        .fs = {
            .images[0] = state.shadow.map,
            .samplers[0] = state.shadow.sampler,
        },
    };
}

void draw_ui(void)
{
    auto windowSize = ImGui::GetWindowSize();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Begin("Offscreen Render");
    ImGui::BeginChild("Offscreen Render");
    ImGui::Image(simgui_imtextureid(state.ui.shadow_map), windowSize, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::EndChild();
    ImGui::End();
}

void frame(void)
{
    batteries::frame();

    draw_ui();

    const float t = (float)(sapp_frame_duration() * 60.0);
    state.ry += 0.2f * sapp_frame_duration();

    const auto width = sapp_width();
    const auto height = sapp_height();

    // rotate suzanne
    state.suzanne.transform.rotation = glm::rotate(state.suzanne.transform.rotation, (float)sapp_frame_duration(), glm::vec3(0.0, 1.0, 0.0));

    // shadow pass matrices
    const glm::mat4 rym = glm::rotate(state.ry, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 light_pos = rym * glm::vec4(10.0f, 10.0f, -10.0f, 1.0f);
    glm::mat4 light_view = glm::lookAt(light_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
    glm::mat4 light_view_proj = light_proj * light_view;

    // display pass matrices
    glm::vec3 eye = glm::vec3(0.0f, 1.5f, 6.0f);
    glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)(width / (float)height), 0.01f, 10.0f);
    glm::mat4 view_proj = proj * view;

    // parameters for drawing a shadow
    const vs_shadow_params_t vs_shadow_params = {
        .model = state.suzanne.transform.matrix(),
        .view_proj = light_view_proj,
    };

    const fs_display_params_t fs_display_params = {
        .light_pos = light_pos,
        .eye_pos = eye,
    };

    // parameters for drawing suzanne
    const vs_display_params_t suzanne_vs_display_params = {
        .model = state.suzanne.transform.matrix(),
        .view_proj = view_proj,
        .light_view_proj = light_view_proj,
    };

    // parameters for drawing the plane
    const vs_display_params_t plane_vs_display_params = {
        .model = glm::mat4(1.0f),
        .view_proj = view_proj,
        .light_view_proj = light_view_proj,
    };

    // render the shadow map
    sg_begin_pass({.action = state.shadow.pass_action, .attachments = state.shadow.attachments});
    sg_apply_pipeline(state.shadow.pip);
    sg_apply_bindings(&state.shadow.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_shadow_params));
    sg_draw(0, state.suzanne.mesh.num_faces * 3, 1);
    sg_end_pass();

    // render the display
    sg_begin_pass({.action = state.display.pass_action, .swapchain = sglue_swapchain()});
    sg_apply_pipeline(state.display.pip);
    sg_apply_bindings(&state.display.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_display_params));
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(suzanne_vs_display_params));
    sg_draw(0, state.suzanne.mesh.num_faces * 3, 1);

    // render plane
    sg_apply_bindings(&state.plane_bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(plane_vs_display_params));
    sg_draw(0, 6, 1);

    // draw ui
    simgui_render();

    sg_end_pass();
    sg_commit();
}

void event(const sapp_event *event)
{
    batteries::event(event);
}

void cleanup(void)
{
    batteries::shutdown();
}

sapp_desc sokol_main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .event_cb = event,
        .cleanup_cb = cleanup,
        .width = 800,
        .height = 800,
        .window_title = "gpr300-sokol",
        .logger.func = slog_func,
    };
}