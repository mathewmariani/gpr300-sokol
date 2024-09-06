#include "depthbuffer.h"

static constexpr int depth_map_size = 1024;

namespace
{
    // clang-format off
    const char depth_vs[] = R"(#version 300 es
        layout(location = 0) in vec3 position;
        uniform mat4 model;
        uniform mat4 view_proj;
        void main()
        {
            gl_Position = view_proj * model * vec4(position, 1.0);
        })";
    const char depth_fs[] = R"(#version 300 es
        precision mediump float;
        void main(){})";
    // clang-format on
}

namespace batteries
{
    Depthbuffer::Depthbuffer()
    {
        depth = sg_make_image({
            .render_target = true,
            .width = depth_map_size,
            .height = depth_map_size,
            .sample_count = 1,
            .pixel_format = SG_PIXELFORMAT_DEPTH,
            .label = "depth-image",
        });

        attachments = sg_make_attachments({
            .depth_stencil.image = depth,
            .label = "depthbuffer-attachments",
        });
    }
}