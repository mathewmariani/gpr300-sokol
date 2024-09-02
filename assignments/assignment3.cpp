#define BOILERPLATE_IMPL
#include "boilerplate.h"

//
// Assignment3 -- Deferred Rendering
//

#include "batteries/assets.h"
#include "batteries/camera.h"
#include "batteries/model.h"
#include "batteries/materials.h"
#include "batteries/lights.h"

#include "batteries/framebuffer.h"
#include "batteries/geometry.h"
#include "batteries/lighting.h"
#include "batteries/gizmo.h"

#include <string>

enum
{
    MAX_INSTANCES = 64,
};

// application state
static struct
{
    uint8_t file_buffer[boilerplate::megabytes(4)];

    batteries::framebuffer_t framebuffer;
    batteries::geometry_t geometry;
    batteries::gizmo_t gizmo;
    batteries::lighting_t lighting;

    sg_attachments gizmo_attachments;

    struct
    {
        simgui_image_t color_img;
        simgui_image_t position_img;
        simgui_image_t normal_img;
        simgui_image_t depth_img;
        float radius;
        float instance_offset;
    } debug;

    int num_instances;
    batteries::camera_t camera;
    batteries::camera_controller_t camera_controller;
    batteries::model_t suzanne;
} state = {
    .num_instances = 9,
};

// instance data buffer;
static glm::mat4 instance_data[MAX_INSTANCES];
static batteries::my_light_t instance_light_data;

void load_suzanne(void)
{
    state.suzanne.mesh.vbuf = sg_alloc_buffer();
    state.suzanne.mesh.bindings = (sg_bindings){
        .vertex_buffers = {
            [0] = state.suzanne.mesh.vbuf,
            [1] = sg_make_buffer({
                .type = SG_BUFFERTYPE_VERTEXBUFFER,
                .data = SG_RANGE(instance_data),
            }),
        },
    };

    batteries::load_obj({
        .buffer_id = state.suzanne.mesh.vbuf,
        .mesh = &state.suzanne.mesh,
        .path = "assets/suzanne.obj",
        .buffer = SG_RANGE(state.file_buffer),
    });
}

static glm::vec4 generateRandomPointOnUnitSphere()
{
    auto u = static_cast<double>(rand()) / RAND_MAX;
    auto v = static_cast<double>(rand()) / RAND_MAX;

    auto phi = 2.0 * M_PI * u;
    auto theta = acos(2.0 * v - 1.0);

    auto x = sin(theta) * cos(phi);
    auto y = sin(theta) * sin(phi);
    auto z = cos(theta);

    return glm::vec4(x, y, z, 1.0);
}

static void init_instance_data(void)
{
    const auto offset = 5.0f;
    const auto radius = 3.0f;
    for (int i = 0, x = 0, y = 0, dx = 0, dy = 0; i < MAX_INSTANCES; i++, x += dx, y += dy)
    {
        // transform
        const auto position = glm::vec3(x, 0.0f, y) * offset;

        // random color
        const auto r = static_cast<float>(((rand() % 100) / 200.0f) + 0.5f);
        const auto g = static_cast<float>(((rand() % 100) / 200.0f) + 0.5f);
        const auto b = static_cast<float>(((rand() % 100) / 200.0f) + 0.5f);

        // suzanne
        glm::mat4 *inst = &instance_data[i];
        *inst = glm::translate(position);

        // lights
        const auto orbit = generateRandomPointOnUnitSphere() * radius;
        instance_light_data.color[i] = {r, g, b, 1.0f};
        instance_light_data.position[i] = glm::vec4(position, 1.0f) + orbit;

        // at a corner?
        if (abs(x) == abs(y))
        {
            if (x >= 0)
            {
                // top-right corner: start a new ring
                if (y >= 0)
                {
                    x += 1;
                    y += 1;
                    dx = 0;
                    dy = -1;
                }
                // bottom-right corner
                else
                {
                    dx = -1;
                    dy = 0;
                }
            }
            else
            {
                // top-left corner
                if (y >= 0)
                {
                    dx = +1;
                    dy = 0;
                }
                // bottom-left corner
                else
                {
                    dx = 0;
                    dy = +1;
                }
            }
        }
    }
}

void init(void)
{
    const auto width = sapp_width();
    const auto height = sapp_height();

    boilerplate::setup();
    batteries::create_framebuffer(&state.framebuffer, width, height);
    batteries::create_geometry_pass(&state.geometry, width, height);
    batteries::create_lighting_pass(&state.lighting, &state.geometry);
    batteries::create_gizmo_pass(&state.gizmo);

    init_instance_data();
    load_suzanne();

    state.geometry.bind = state.suzanne.mesh.bindings;

    state.gizmo_attachments = sg_make_attachments({
        .colors[0].image = state.framebuffer.color,
        .depth_stencil.image = state.geometry.depth_img,
        .label = "gizmo-attachment",
    });

    // create an sokol-imgui wrapper geometry render targets
    auto dbg_smp = sg_make_sampler({
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .label = "ui-sampler",
    });

    state.debug.color_img = simgui_make_image({
        .image = state.geometry.color_img,
        .sampler = dbg_smp,
    });
    state.debug.position_img = simgui_make_image({
        .image = state.geometry.position_img,
        .sampler = dbg_smp,
    });
    state.debug.normal_img = simgui_make_image({
        .image = state.geometry.normal_img,
        .sampler = dbg_smp,
    });
    state.debug.depth_img = simgui_make_image({
        .image = state.geometry.depth_img,
        .sampler = dbg_smp,
    });
}

void draw_ui(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%.1fms %.0fFPS | AVG: %.2fms %.1fFPS", ImGui::GetIO().DeltaTime * 1000, 1.0f / ImGui::GetIO().DeltaTime, 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::SliderInt("Num Instances", &state.num_instances, 1, MAX_INSTANCES);
    ImGui::SliderFloat("Instance Spacing", &state.debug.instance_offset, 3.0f, 10.0f);
    ImGui::End();

    ImGui::Begin("Offscreen Render");
    ImGui::BeginChild("Offscreen Render");
    auto size = ImGui::GetWindowSize();
    ImGui::Image(simgui_imtextureid(state.debug.position_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::Image(simgui_imtextureid(state.debug.normal_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::Image(simgui_imtextureid(state.debug.color_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::Image(simgui_imtextureid(state.debug.depth_img), size, {0.0f, 1.0f}, {1.0f, 0.0f});
    ImGui::EndChild();
    ImGui::End();
}

void frame(void)
{
    boilerplate::frame();
    draw_ui();

    const auto t = (float)sapp_frame_duration();
    state.camera_controller.update(&state.camera, t);

    const auto view_proj = state.camera.projection() * state.camera.view();

    // initialize uniform data
    const batteries::vs_geometry_params_t vs_geometry_params = {
        .view_proj = view_proj,
    };
    const batteries::fs_lighting_params_t fs_lighting_params = {
        .camera_position = state.camera.position,
        .lights = instance_light_data,
        .num_instances = state.num_instances,
    };

    // render the geometry pass
    sg_begin_pass(&state.geometry.pass);
    sg_apply_pipeline(state.geometry.pip);
    sg_apply_bindings(&state.geometry.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_geometry_params));
    sg_draw(0, state.suzanne.mesh.num_faces * 3, state.num_instances);
    sg_end_pass();

    // render the lighting pass
    sg_begin_pass(&state.framebuffer.pass);
    sg_apply_pipeline(state.lighting.pip);
    sg_apply_bindings(&state.lighting.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_lighting_params));
    sg_draw(0, 6, 1);
    sg_end_pass();

    // render the gizmo pass
    sg_begin_pass({.action = state.gizmo.action, .attachments = state.gizmo_attachments});
    sg_apply_pipeline(state.gizmo.pip);
    sg_apply_bindings(&state.gizmo.bind);
    for (auto i = 0; i < state.num_instances; i++)
    {
        // parameters for the gizmo pass
        const batteries::vs_gizmo_params_t vs_gizmo_params = {
            .view_proj = view_proj,
            .model = glm::translate(glm::mat4(1.0f), glm::vec3(instance_light_data.position[i])),
        };
        // parameters for the gizmo pass
        const batteries::fs_gizmo_light_params_t fs_gizmo_light_params = {
            .light_color = instance_light_data.color[i],
        };
        sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_gizmo_params));
        sg_apply_uniforms(SG_SHADERSTAGE_FS, 0, SG_RANGE(fs_gizmo_light_params));
        sg_draw(state.gizmo.sphere.base_element, state.gizmo.sphere.num_elements, 1);
    }
    sg_end_pass();

    // render framebuffer
    sg_begin_pass({.action = state.framebuffer.action, .swapchain = sglue_swapchain()});
    sg_apply_pipeline(state.framebuffer.pip);
    sg_apply_bindings(&state.framebuffer.bind);
    sg_draw(0, 6, 1);

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