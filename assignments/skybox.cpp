#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Assignment0 -- Blinn Phong
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"
#include "batteries/materials.h"
#include "batteries/lights.h"

// shaders
#include "shaders/blinn_phong.h"
#include "shaders/skybox.h"

typedef struct
{
    glm::mat4 view_proj;
    glm::mat4 model;
    glm::vec3 eye;
    glm::vec3 ambient;
    glm::vec3 light_dir;
} vs_display_params_t;

typedef struct
{
    glm::mat4 view_proj;
} vs_skybox_params_t;

typedef struct
{
    float Ka, Kd, Ks;
    float Shininess;
} fs_display_params_t;

// application state
static struct
{
    struct
    {
        sg_pass_action pass_action;
        sg_pass pass;
        sg_pipeline pip;
        sg_bindings bind;
    } display;

    struct
    {
        sg_pipeline pip;
        sg_bindings bind;
    } skybox;

    struct
    {
        float ry;
        glm::vec3 ambient_light;
        boilerplate::model_t suzanne;
        boilerplate::material_t material;
    } scene;

    uint8_t file_buffer[boilerplate::megabytes(10)];
    uint8_t cubemap_buffer[boilerplate::megabytes(10)];
    uint8_t vertex_buffer[boilerplate::megabytes(10)];
} state = {
    .scene = {
        .ry = 0.0f,
        .ambient_light = glm::vec3(0.25f, 0.45f, 0.65f),
        .material = {
            .Ka = 1.0f,
            .Kd = 0.5f,
            .Ks = 0.5f,
            .Shininess = 128.0f,
        },
    },
};

static void fail_callback()
{
    std::printf("[!!!] fail_ballback for assets\n");
}

void create_display_pass(void)
{
    auto shader_desc = (sg_shader_desc){
        .vs = {
            .source = blinn_phong_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(vs_display_params_t),
                .uniforms = {
                    [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                    [1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                    [2] = {.name = "eye", .type = SG_UNIFORMTYPE_FLOAT3},
                    [3] = {.name = "ambient", .type = SG_UNIFORMTYPE_FLOAT3},
                    [4] = {.name = "light_dir", .type = SG_UNIFORMTYPE_FLOAT3},
                },
            },
        },
        .fs = {
            .source = blinn_phong_fs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(fs_display_params_t),
                .uniforms = {
                    [0] = {.name = "material.Ka", .type = SG_UNIFORMTYPE_FLOAT},
                    [1] = {.name = "material.Kd", .type = SG_UNIFORMTYPE_FLOAT},
                    [2] = {.name = "material.Ks", .type = SG_UNIFORMTYPE_FLOAT},
                    [3] = {.name = "material.Shininess", .type = SG_UNIFORMTYPE_FLOAT},
                },
            },
        },
    };

    state.display.pass_action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {state.scene.ambient_light.r, state.scene.ambient_light.g, state.scene.ambient_light.b, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        }};

    state.display.pip = sg_make_pipeline((sg_pipeline_desc){
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT3,
                [1].format = SG_VERTEXFORMAT_FLOAT3,
            }},
        .shader = sg_make_shader(shader_desc),
        .index_type = SG_INDEXTYPE_NONE,
        .face_winding = SG_FACEWINDING_CCW,
        .cull_mode = SG_CULLMODE_BACK,
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true,
        },
        .label = "suzanne-pipeline",
    });

    sg_buffer vbuf = sg_alloc_buffer();
    state.display.bind = (sg_bindings){
        .vertex_buffers[0] = vbuf,
    };

    batteries::load_obj((batteries::obj_request_t){
        .buffer_id = vbuf,
        .mesh = &state.scene.suzanne.mesh,
        .path = "assets/objects/coin/coin.obj",
        .buffer = SG_RANGE(state.file_buffer),
    });
}

void create_skybox_pass(void)
{
    // clang-format off
  float vertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
  };
    // clang-format on

    sg_buffer skybox_vbuf = sg_make_buffer((sg_buffer_desc){
        .data = SG_RANGE(vertices),
        .label = "skybox-vertices",
    });

    auto shader_desc = (sg_shader_desc){
        .vs = {
            .source = skybox_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(vs_skybox_params_t),
                .uniforms = {
                    [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                },
            },
        },
        .fs = {
            .source = skybox_fs,
            .images[0] = {.used = true, .image_type = SG_IMAGETYPE_CUBE},
            .samplers[0] = {.used = true},
            .image_sampler_pairs[0] = {
                .glsl_name = "skybox",
                .image_slot = 0,
                .sampler_slot = 0,
                .used = true,
            },
        },
    };

    state.skybox.pip = sg_make_pipeline((sg_pipeline_desc){
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT3,
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
        .label = "skybox-pipeline",
    });

    sg_image skybox_img = sg_alloc_image();
    sg_sampler skybox_smp = sg_make_sampler((sg_sampler_desc){
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_w = SG_WRAP_CLAMP_TO_EDGE,
        .label = "skybox-sampler",
    });
    state.skybox.bind = (sg_bindings){
        .vertex_buffers[0] = skybox_vbuf,
        .fs = {
            .images[0] = skybox_img,
            .samplers[0] = skybox_smp,
        },
    };

    batteries::load_cubemap((batteries::cubemap_request_t){
        .img_id = skybox_img,
        .path_right = "assets/skybox/right.jpg",
        .path_left = "assets/skybox/left.jpg",
        .path_top = "assets/skybox/top.jpg",
        .path_bottom = "assets/skybox/bottom.jpg",
        .path_front = "assets/skybox/front.jpg",
        .path_back = "assets/skybox/back.jpg",
        .buffer_ptr = state.cubemap_buffer,
        .buffer_offset = 1024 * 1024,
    });
}

void init(void)
{
    boilerplate::setup();

    create_display_pass();
    create_skybox_pass();
}

void frame(void)
{
    // boilerplate::frame();
    sfetch_dowork();

    const auto t = (float)sapp_frame_duration();
    state.scene.ry += 0.2f * t;

    const auto width = sapp_width();
    const auto height = sapp_height();

    // math required by the scene
    auto camera_pos = glm::vec3(0.0f, 1.5f, 6.0f);
    auto camera_proj = glm::perspective(glm::radians(60.0f), (float)(width / (float)height), 0.01f, 10.0f);
    auto camera_view = glm::lookAt(camera_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    auto camera_view_proj = camera_proj * camera_view;

    // sugar: rotate suzzane
    state.scene.suzanne.transform.rotation = glm::rotate(state.scene.suzanne.transform.rotation, t, glm::vec3(0.0, 1.0, 0.0));

    // sugar: rotate light
    const glm::mat4 rym = glm::rotate(state.scene.ry, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 light_pos = rym * glm::vec4(50.0f, 50.0f, -50.0f, 1.0f);

    // initialize uniform data
    const vs_display_params_t vs_display_params = {
        .view_proj = camera_view_proj,
        .model = state.scene.suzanne.transform.matrix(),
        .eye = camera_pos,
        .ambient = state.scene.ambient_light,
        .light_dir = glm::normalize(light_pos),
    };
    const fs_display_params_t fs_display_params = {
        .Ka = state.scene.material.Ka,
        .Kd = state.scene.material.Kd,
        .Ks = state.scene.material.Ks,
        .Shininess = state.scene.material.Shininess,
    };
    const vs_skybox_params_t vs_skybox_params = {
        .view_proj = glm::mat4(glm::mat3(camera_view_proj)),
    };

    // suzanne pass
    sg_begin_default_pass(&state.display.pass_action, width, height);

    // suzanne pass
    sg_apply_pipeline(state.display.pip);
    sg_apply_bindings(&state.display.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_display_params));
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_display_params));
    sg_draw(0, state.scene.suzanne.mesh.num_faces, 1);

    // skybox pass
    sg_apply_pipeline(state.skybox.pip);
    sg_apply_bindings(&state.skybox.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_skybox_params));
    sg_draw(0, 36, 1);

    sg_end_pass();
    sg_commit();
}

void event(const sapp_event *event)
{
    boilerplate::event(event);
}

void cleanup(void)
{
    boilerplate::shutdown();
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