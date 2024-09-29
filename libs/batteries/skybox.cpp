#include "assets.h"
#include "skybox.h"
#include "shaders/skybox.h"

namespace batteries
{
    static uint8_t cubemap_buffer[1024 * 1024 * 10];

    Skybox::Skybox()
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

        pip = sg_make_pipeline({
            .layout = {
                .attrs = {
                    [0].format = SG_VERTEXFORMAT_FLOAT3,
                },
            },
            .shader = sg_make_shader({
                .vs = {
                    .source = skybox_vs,
                    .uniform_blocks[0] = {
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
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
            }),
            .index_type = SG_INDEXTYPE_NONE,
            .face_winding = SG_FACEWINDING_CCW,
            .cull_mode = SG_CULLMODE_BACK,
            .depth = {
                .pixel_format = SG_PIXELFORMAT_DEPTH,
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
            },
            .label = "skybox-pipeline",
        });

        auto skybox_img = sg_alloc_image();
        bind = (sg_bindings){
            .vertex_buffers[0] = sg_make_buffer({
                .data = SG_RANGE(vertices),
                .label = "skybox-vertices",
            }),
            .fs = {
                .images[0] = skybox_img,
                .samplers[0] = sg_make_sampler({
                    .min_filter = SG_FILTER_LINEAR,
                    .mag_filter = SG_FILTER_LINEAR,
                    .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
                    .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
                    .wrap_w = SG_WRAP_CLAMP_TO_EDGE,
                    .label = "skybox-sampler",
                }),
            },
        };

        batteries::load_cubemap({
            .img_id = skybox_img,
            .path_right = "assets/skybox/right.jpg",
            .path_left = "assets/skybox/left.jpg",
            .path_top = "assets/skybox/top.jpg",
            .path_bottom = "assets/skybox/bottom.jpg",
            .path_front = "assets/skybox/front.jpg",
            .path_back = "assets/skybox/back.jpg",
            .buffer_ptr = cubemap_buffer,
            .buffer_offset = 1024 * 1024,
        });
    }

    void Skybox::Render(const vs_params_t vs_params)
    {
        // render skybox
        sg_apply_pipeline(pip);
        sg_apply_bindings(&bind);
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_params));
        sg_draw(0, 36, 1);
    }
}