#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
    struct Geometrybuffer
    {
        sg_pass pass;
        sg_image color_img;
        sg_image position_img;
        sg_image normal_img;
        sg_image depth_img;

        Geometrybuffer(int width, int height)
        {
            sg_image_desc img_desc = {
                .render_target = true,
                .width = width,
                .height = height,
                .sample_count = 1,
            };

            img_desc.pixel_format = SG_PIXELFORMAT_RGBA16F;
            position_img = sg_make_image(&img_desc);

            img_desc.pixel_format = SG_PIXELFORMAT_RGBA16F;
            normal_img = sg_make_image(&img_desc);

            img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
            color_img = sg_make_image(&img_desc);

            img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
            depth_img = sg_make_image(&img_desc);

            pass = (sg_pass){
                .action = (sg_pass_action){
                    .colors = {
                        [0] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 0.0f}},
                        [1] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 0.0f}},
                        [2] = {.load_action = SG_LOADACTION_CLEAR, .clear_value = {0.0f, 0.0f, 0.0f, 0.0f}},
                    },
                    .depth = {
                        .load_action = SG_LOADACTION_CLEAR,
                        .store_action = SG_STOREACTION_STORE,
                        .clear_value = 1.0f,
                    },
                },
                .attachments = sg_make_attachments({
                    .colors = {
                        [0].image = position_img,
                        [1].image = normal_img,
                        [2].image = color_img,
                    },
                    .depth_stencil.image = depth_img,
                    .label = "geometry-pass",
                }),
            };
        }
    };
}