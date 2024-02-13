#define BATTERIES_IMPL
#include "batteries.h"

//
// Assignment0 -- Blinn Phong
//

// shaders
#include "shaders/pbr.h"

typedef struct
{
    glm::mat4 view_proj;
    glm::mat4 model;
    glm::vec3 camera_pos;
    glm::vec3 light_pos;
    glm::vec3 ambient;
} vs_display_params_t;

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
        float ry;
        glm::vec3 ambient_light;
        batteries::model_t suzanne;
        batteries::material_t material;
    } scene;

    uint8_t file_buffer[batteries::megabytes(5)];
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

void create_display_pass(void)
{
    auto shader_desc = (sg_shader_desc){
        .vs = {
            .source = pbr_vs,
            .uniform_blocks[0] = {
                .layout = SG_UNIFORMLAYOUT_NATIVE,
                .size = sizeof(vs_display_params_t),
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
            .source = pbr_fs,
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

    state.display.pass_action = (sg_pass_action){
        .colors[0] = {
            .clear_value = {state.scene.ambient_light.r, state.scene.ambient_light.g, state.scene.ambient_light.b, 1.0f},
            .load_action = SG_LOADACTION_CLEAR,
        }};

    state.display.pip = sg_make_pipeline((sg_pipeline_desc){
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
        .label = "display-pipeline",
    });

    auto vbuf = sg_alloc_buffer();
    state.display.bind = (sg_bindings){
        .vertex_buffers[0] = vbuf,
    };

    batteries::assets::load_obj((batteries::assets::obj_request_t){
        .buffer_id = vbuf,
        .mesh = &state.scene.suzanne.mesh,
        .path = "assets/objects/togezoshell/togezoshell.obj",
        .buffer = SG_RANGE(state.file_buffer),
    });

    sg_sampler smp = sg_make_sampler((sg_sampler_desc){
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .wrap_u = SG_WRAP_REPEAT,
        .wrap_v = SG_WRAP_REPEAT,
        .label = "model-sampler",
    });

    auto col = sg_alloc_image();
    auto mtl = sg_alloc_image();
    auto rgh = sg_alloc_image();
    auto ao = sg_alloc_image();
    auto spc = sg_alloc_image();

    state.display.bind.fs = {
        .images = {[0] = col, [1] = mtl, [2] = rgh, [3] = ao, [4] = spc},
        .samplers[0] = smp,
    };

    batteries::assets::load_img((batteries::assets::img_request_t){
        .image_id = ao,
        .path = "assets/materials/togezoshell/togezoshell_ao.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
    batteries::assets::load_img((batteries::assets::img_request_t){
        .image_id = col,
        .path = "assets/materials/togezoshell/togezoshell_col.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
    batteries::assets::load_img((batteries::assets::img_request_t){
        .image_id = mtl,
        .path = "assets/materials/togezoshell/togezoshell_mtl.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
    batteries::assets::load_img((batteries::assets::img_request_t){
        .image_id = rgh,
        .path = "assets/materials/togezoshell/togezoshell_rgh.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
    batteries::assets::load_img((batteries::assets::img_request_t){
        .image_id = spc,
        .path = "assets/materials/togezoshell/togezoshell_spc.png",
        .buffer = SG_RANGE(state.file_buffer),
    });
}

void init(void)
{
    batteries::setup();

    create_display_pass();
}

void frame(void)
{
    batteries::frame();

    const auto t = (float)sapp_frame_duration();
    state.scene.ry += 0.2f * t;

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    if (ImGui::ColorEdit3("Ambient Light", &state.scene.ambient_light[0]))
    {
        state.display.pass_action.colors[0].clear_value = {state.scene.ambient_light.r, state.scene.ambient_light.g, state.scene.ambient_light.b, 1.0f};
    }
    if (ImGui::CollapsingHeader("Material"))
    {
        ImGui::SliderFloat("Ambient", &state.scene.material.Ka, 0.0f, 1.0f);
        ImGui::SliderFloat("Diffuse", &state.scene.material.Kd, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular", &state.scene.material.Ks, 0.0f, 1.0f);
        ImGui::SliderFloat("Shininess", &state.scene.material.Shininess, 2.0f, 1024.0f);
    }
    ImGui::End();

    const auto width = sapp_width();
    const auto height = sapp_height();

    // math required by the scene
    auto camera_pos = glm::vec3(0.0f, 1.5f, 6.0f);
    auto camera_proj = glm::perspective(glm::radians(60.0f), (float)(width / (float)height), 0.01f, 10.0f);
    auto camera_view = glm::lookAt(camera_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    auto camera_view_proj = camera_proj * camera_view;

    // sugar: rotate suzzane
    state.scene.suzanne.transform.rotation = glm::rotate(state.scene.suzanne.transform.rotation, t, glm::vec3(0.0, 1.0, 0.0));
    state.scene.suzanne.transform.scale = glm::vec3(0.5f);

    // sugar: rotate light
    const glm::mat4 rym = glm::rotate(state.scene.ry, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 light_pos = rym * glm::vec4(10.0f, 0.0f, -10.0f, 1.0f);

    // initialize uniform data
    const vs_display_params_t vs_params = {
        .view_proj = camera_view_proj,
        .model = state.scene.suzanne.transform.matrix(),
        .camera_pos = camera_pos,
        .light_pos = light_pos,
        .ambient = state.scene.ambient_light,
    };

    // graphics pass
    sg_begin_default_pass(&state.display.pass_action, width, height);
    sg_apply_pipeline(state.display.pip);
    sg_apply_bindings(&state.display.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
    sg_draw(0, state.scene.suzanne.mesh.num_faces * 3, 1);

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