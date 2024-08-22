#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Assignment2 -- Shadow Mapping
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"
#include "batteries/materials.h"
#include "batteries/lights.h"

// shaders
#include "shaders/shadow_depth.h"
#include "shaders/shadow_map.h"
#include "shaders/no_post_process.h"
#include "shaders/shapes.h"

#include <string>

static constexpr glm::vec4 light_orbit_radius = glm::vec4(2.0f, 2.0f, -2.0f, 1.0f);

enum
{
    DEPTH_MAP_SIZE = 2048,
};

typedef struct
{
    float brightness;
    glm::vec3 color;
    glm::vec3 position;
} light_t;

typedef struct
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
} material_t;

typedef struct
{
    glm::mat4 model;
    glm::mat4 view_proj;
} vs_depth_params_t;

typedef struct
{
    glm::mat4 model;
    glm::mat4 view_proj;
    glm::mat4 light_view_proj;
} vs_shadow_params_t;

typedef struct
{
    glm::vec3 light_pos;
    glm::vec3 eye_pos;
    material_t material;
    batteries::ambient_t ambient;
} fs_shadow_params_t;

typedef struct
{
    glm::mat4 view_proj;
    glm::mat4 model;
} vs_gizmo_params_t;

typedef struct
{
    glm::vec3 light_color;
} fs_gizmo_light_params_t;

// application state
static struct
{
    struct
    {
        sg_attachments attachments;
        sg_image color;
        sg_image depth;
        sg_sampler sampler;
    } framebuffer;

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
        sg_image img;
        sg_sampler smp;
    } depth;

    struct
    {
        sg_pass_action pass_action;
        sg_pipeline pip;
        sg_bindings bind;
    } shadow;

    struct
    {
        sg_pass_action pass_action;
        sg_pipeline pip;
        sg_bindings bind;
        sshape_element_range_t sphere;
    } gizmo;

    struct
    {
        simgui_image_t shadow_map;
    } ui;

    uint8_t file_buffer[boilerplate::megabytes(4)];

    batteries::camera_t camera;
    batteries::camera_controller_t camera_controller;
    batteries::ambient_t ambient;
    light_t light;

    struct
    {
        float ry;
        batteries::model_t suzanne;
        material_t material;
        sg_buffer plane_vbuf;
        sg_bindings plane_bind;
    } scene;
} state = {
    .light = {
        .brightness = 1.0f,
        .color = glm::vec3(1.0f, 1.0f, 1.0f),
    },
    .ambient = {
        .intensity = 0.3f,
        .color = {0.25f, 0.45f, 0.65f},
        .direction = {0.0f, 0.0f, 0.0f},
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

void create_framebuffer(void)
{
    const auto width = sapp_width();
    const auto height = sapp_height();

    // color attachment
    state.framebuffer.color = sg_make_image({
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .render_target = true,
        .width = width,
        .height = height,
        .label = "framebuffer-color-image",
    });

    // depth attachment
    state.framebuffer.depth = sg_make_image({
        .pixel_format = SG_PIXELFORMAT_DEPTH,
        .render_target = true,
        .width = width,
        .height = height,
        .label = "framebuffer-depth-image",
    });

    // create an image sampler
    state.framebuffer.sampler = sg_make_sampler({
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
    });

    state.framebuffer.attachments = sg_make_attachments({
        .colors[0].image = state.framebuffer.color,
        .depth_stencil.image = state.framebuffer.depth,
        .label = "framebuffer-attachment",
    });
}

void create_display_pass()
{
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
            .source = no_post_process_vs,
        },
        .fs = {
            .source = no_post_process_fs,
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

void create_depth_pass(void)
{
    // create a texture with only a depth attachment
    state.depth.img = sg_make_image({
        .render_target = true,
        .width = DEPTH_MAP_SIZE,
        .height = DEPTH_MAP_SIZE,
        .sample_count = 1,
        .pixel_format = SG_PIXELFORMAT_DEPTH,
        .label = "depth-image",
    });

    // create an image sampler
    state.depth.smp = sg_make_sampler({
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .compare = SG_COMPAREFUNC_LESS,
        .label = "shadow-sampler",
    });

    state.depth.attachments = sg_make_attachments({
        .depth_stencil.image = state.depth.img,
        .label = "shadow-pass",
    });

    auto shadow_shader_desc = (sg_shader_desc){
        .vs = {
            .source = shadow_depth_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(vs_depth_params_t),
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

    state.depth.pass_action = (sg_pass_action){
        .depth = {
            .load_action = SG_LOADACTION_CLEAR,
            .store_action = SG_STOREACTION_STORE,
            .clear_value = 1.0f,
        },
    };

    state.depth.pip = sg_make_pipeline({
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

    state.depth.bind = (sg_bindings){
        .vertex_buffers[0] = state.scene.suzanne.mesh.vbuf,
    };
}

void create_shadow_pass(void)
{
    auto shader_desc = (sg_shader_desc){
        .vs = {
            .source = shadow_map_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(vs_shadow_params_t),
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
                .size = sizeof(fs_shadow_params_t),
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
    state.shadow.pass_action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        },
    };
    state.shadow.pip = sg_make_pipeline({
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
        .label = "display-pipeline",
    });
    state.shadow.bind = (sg_bindings){
        .vertex_buffers[0] = state.scene.suzanne.mesh.vbuf,
        .fs = {
            .images[0] = state.depth.img,
            .samplers[0] = state.depth.smp,
        },
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

void init(void)
{
    boilerplate::setup();

    load_suzanne();
    create_framebuffer();
    create_display_pass();
    create_depth_pass();
    create_shadow_pass();
    create_gizmo_pass();

    // create an sokol-imgui wrapper for the shadow map
    auto ui_smp = sg_make_sampler((sg_sampler_desc){
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .label = "ui-sampler",
    });
    state.ui.shadow_map = simgui_make_image((simgui_image_desc_t){
        .image = state.depth.img,
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

    state.scene.plane_vbuf = sg_make_buffer({
        .data = SG_RANGE(plane_vertices),
        .label = "plane-vertices",
    });

    state.scene.plane_bind = (sg_bindings){
        .vertex_buffers[0] = state.scene.plane_vbuf,
        .fs = {
            .images[0] = state.depth.img,
            .samplers[0] = state.depth.smp,
        },
    };
}

void draw_ui(void)
{
    auto windowSize = ImGui::GetWindowSize();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (ImGui::CollapsingHeader("Ambient Light"))
    {
        if (ImGui::SliderFloat("Intensity", &state.ambient.intensity, 0.0f, 1.0f))
        {
            state.shadow.pass_action.colors[0].clear_value = {
                state.ambient.color.r * state.ambient.intensity,
                state.ambient.color.g * state.ambient.intensity,
                state.ambient.color.b * state.ambient.intensity,
                1.0f,
            };
        }
        ImGui::DragFloat3("Direction", &state.ambient.direction[0], 0.01f, -1.0f, 1.0f);
        if (ImGui::ColorEdit3("Color", &state.ambient.color[0]))
        {
            state.shadow.pass_action.colors[0].clear_value = {
                state.ambient.color.r * state.ambient.intensity,
                state.ambient.color.g * state.ambient.intensity,
                state.ambient.color.b * state.ambient.intensity,
                1.0f,
            };
        }
    }
    if (ImGui::CollapsingHeader("Suzanne"))
    {
        ImGui::DragFloat3("Position", &state.scene.suzanne.transform.position[0], 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("Rotation", &state.scene.suzanne.transform.rotation[0], 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("Scale", &state.scene.suzanne.transform.scale[0], 0.01f, 0.0f, 1.0f);
    }
    if (ImGui::CollapsingHeader("Material"))
    {
        ImGui::SliderFloat3("Ambient", &state.scene.material.ambient[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Diffuse", &state.scene.material.diffuse[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Specular", &state.scene.material.specular[0], 0.0f, 1.0f);
        ImGui::SliderFloat("Shininess", &state.scene.material.shininess, 2.0f, 1024.0f);
    }
    ImGui::End();

    ImGui::Begin("Offscreen Render");
    ImGui::BeginChild("Offscreen Render");
    ImGui::Image(simgui_imtextureid(state.ui.shadow_map), windowSize, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::EndChild();
    ImGui::End();
}

void frame(void)
{
    boilerplate::frame();
    draw_ui();

    const auto t = (float)sapp_frame_duration();
    state.camera_controller.update(&state.camera, t);
    state.scene.ry += 0.2f * t;

    // update the camera controller
    state.camera_controller.update(&state.camera, sapp_frame_duration());

    // sugar: rotate light
    const auto rym = glm::rotate(state.scene.ry, glm::vec3(0.0f, 1.0f, 0.0f));
    state.light.position = rym * light_orbit_radius;

    // depth pass matrices
    const auto light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
    const auto light_view = glm::lookAt(state.light.position, {0.0f, 0.0f, 0.0f}, glm::vec3(0.0f, 1.0f, 0.0f));
    const auto light_view_proj = light_proj * light_view;

    // shadow pass matrices
    const auto view_proj = state.camera.projection() * state.camera.view();

    // initialize uniform data
    const vs_depth_params_t vs_shadow_params = {
        .model = state.scene.suzanne.transform.matrix(),
        .view_proj = light_view_proj,
    };
    const vs_shadow_params_t suzanne_vs_display_params = {
        .model = state.scene.suzanne.transform.matrix(),
        .view_proj = view_proj,
        .light_view_proj = light_view_proj,
    };
    const fs_shadow_params_t fs_display_params = {
        .light_pos = state.light.position,
        .eye_pos = state.camera.position,
        .material = state.scene.material,
        .ambient = state.ambient,
    };
    const vs_shadow_params_t plane_vs_display_params = {
        .model = glm::mat4(1.0f),
        .view_proj = view_proj,
        .light_view_proj = light_view_proj,
    };
    const vs_gizmo_params_t vs_gizmo_params = {
        .view_proj = view_proj,
        .model = glm::translate(glm::mat4(1.0f), state.light.position),
    };
    const fs_gizmo_light_params_t fs_gizmo_light_params = {
        .light_color = state.light.color,
    };

    // step 1:
    // render depth of scene to texture (from light's perspective).
    sg_begin_pass({.action = state.depth.pass_action, .attachments = state.depth.attachments});
    sg_apply_pipeline(state.depth.pip);
    sg_apply_bindings(&state.depth.bind);

    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_shadow_params));
    sg_draw(0, state.scene.suzanne.mesh.num_faces * 3, 1);
    sg_end_pass();

    // step 2:
    // render scene as normal using the generated depth map.
    sg_begin_pass({.action = state.shadow.pass_action, .attachments = state.framebuffer.attachments});
    sg_apply_pipeline(state.shadow.pip);
    sg_apply_bindings(&state.shadow.bind);

    // render suzanne
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_display_params));
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(suzanne_vs_display_params));
    sg_draw(0, state.scene.suzanne.mesh.num_faces * 3, 1);

    // render plane
    sg_apply_bindings(&state.scene.plane_bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(plane_vs_display_params));
    sg_draw(0, 6, 1);
    sg_end_pass();

    // render light sources.
    sg_begin_pass({.action = state.gizmo.pass_action, .attachments = state.framebuffer.attachments});
    sg_apply_pipeline(state.gizmo.pip);
    sg_apply_bindings(&state.gizmo.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_gizmo_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_gizmo_light_params));
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