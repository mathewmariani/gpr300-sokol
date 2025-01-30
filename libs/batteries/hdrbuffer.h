#pragma once

// sokol
#include "sokol/sokol_gfx.h"

namespace batteries
{
    struct HDRbuffer
    {
        sg_pass pass;
        sg_sampler sampler;
        sg_image color_img;
        sg_image bright_img;

        HDRbuffer(int width, int height)
        {
            sg_image_desc img_desc = {
                .render_target = true,
                .width = width,
                .height = height,
                .sample_count = 1,
            };

            img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
            color_img = sg_make_image(&img_desc);

            img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
            bright_img = sg_make_image(&img_desc);

            img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
            depth_img = sg_make_image(&img_desc);

            sampler = sg_make_sampler({
                .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
                .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
                .min_filter = SG_FILTER_LINEAR,
                .mag_filter = SG_FILTER_LINEAR,
            });

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
                        [0].image = color_img,
                        [1].image = bright_img,
                    },
                    .depth_stencil.image = depth_img,
                    .label = "hdr-pass",
                }),
            };
        }
    };
}