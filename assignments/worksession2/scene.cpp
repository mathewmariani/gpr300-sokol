#include "scene.h"

#include "batteries/assets.h"

#include "imgui/imgui.h"
#include "sokol/sokol_imgui.h"
#include "sokol/sokol_shape.h"

#include <unordered_map>

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};
static uint8_t file_buffer[1024 * 1024 * 5];

Scene::Scene()
{
    auto init_terrain = [this]()
    {
        terrain_obj.img = sg_alloc_image();
        sg_sampler smp = sg_make_sampler({
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
            .wrap_u = SG_WRAP_REPEAT,
            .wrap_v = SG_WRAP_REPEAT,
            .label = "heightmap-sampler",
        });

        // generate shape geometries
        auto sizes = sshape_plane_sizes(100);
        std::vector<sshape_vertex_t> vertices(sizes.vertices.num);
        std::vector<uint16_t> indices(sizes.indices.num);

        sshape_buffer_t buf = {
            .vertices.buffer = {.ptr = vertices.data(), .size = sizes.vertices.size},
            .indices.buffer = {.ptr = indices.data(), .size = sizes.indices.size},
        };

        sshape_plane_t plane = {
            .width = 100.0f,
            .depth = 100.0f,
            .tiles = 100,
        };

        buf = sshape_build_plane(&buf, &plane);
        terrain_obj.plane.draw = sshape_element_range(&buf);

        terrain_obj.plane.transform.position = glm::vec3(0.0f, -1.0f, 0.0f);

        // one vertex/index-buffer-pair for all shapes
        const auto vbuf_desc = sshape_vertex_buffer_desc(&buf);
        const auto ibuf_desc = sshape_index_buffer_desc(&buf);

        terrain_obj.bind = (sg_bindings){
            .vs = {
                .images[0] = terrain_obj.img,
                .samplers[0] = smp,
            },
            .fs = {
                .images[0] = terrain_obj.img,
                .samplers[0] = smp,
            },
            .vertex_buffers[0] = sg_make_buffer(&vbuf_desc),
            .index_buffer = sg_make_buffer(&ibuf_desc),
        };

        batteries::load_img({
            .image_id = terrain_obj.img,
            .path = "assets/materials/heightmap.png",
            .buffer = SG_RANGE(file_buffer),
        });
    };

    init_terrain();
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);
}

void Scene::Render(void)
{
    const auto view_proj = camera.projection() * camera.view();

    // initialize uniform data
    const Terrain::vs_params_t vs_terrain_params = {
        .view_proj = view_proj,
        .model = terrain_obj.plane.transform.matrix(),
        .camera_pos = camera.position,
        .scale = 10.0f,
    };

    sg_begin_pass({.action = pass_action, .attachments = framebuffer.attachments});

    // render using blinn-phong pipeline
    terrain.Render(vs_terrain_params, terrain_obj);

    sg_end_pass();

    framebuffer.Render();

    // sg_end_pass();
    // sg_commit();
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);
    ImGui::End();
}