#include "scene.h"
#include "imgui/imgui.h"
#include "batteries/assets.h"

static glm::vec4 light_orbit_radius = {2.0f, 0.0f, 2.0f, 1.0f};
static uint8_t file_buffer[1024 * 1024 * 5];

static IslandPass::Palette pal[]{
    {.highlight = {1.00f, 1.00f, 1.00f}, .shadow = {0.60f, 0.54f, 0.52f}},
    {.highlight = {0.47f, 0.58f, 0.68f}, .shadow = {0.32f, 0.39f, 0.57f}},
    {.highlight = {0.62f, 0.69f, 0.67f}, .shadow = {0.50f, 0.55f, 0.50f}},
    {.highlight = {0.24f, 0.36f, 0.54f}, .shadow = {0.25f, 0.31f, 0.31f}},
};

Scene::Scene()
{
    auto load_island = [this]()
    {
        island.model.mesh.vbuf = sg_alloc_buffer();
        island.model.textures.reserve(8);
        island.model.mesh.bindings = (sg_bindings){
            .vertex_buffers[0] = island.model.mesh.vbuf,
            .fs = {
                .samplers[0] = sg_make_sampler({
                    .min_filter = SG_FILTER_LINEAR,
                    .mag_filter = SG_FILTER_LINEAR,
                    .wrap_u = SG_WRAP_REPEAT,
                    .wrap_v = SG_WRAP_REPEAT,
                    .label = "object-sampler",
                }),
            },
        };
        batteries::load_obj({
            .buffer_id = island.model.mesh.vbuf,
            .mesh = &island.model.mesh,
            .path = "assets/island/Island.obj",
            .buffer = SG_RANGE(file_buffer),
        });

#define LOAD_ISLAND_IMAGE(i, filepath)                         \
    island.model.textures[i] = sg_alloc_image();               \
    batteries::load_img({.image_id = island.model.textures[i], \
                         .path = filepath,                     \
                         .buffer = SG_RANGE(file_buffer)})
        LOAD_ISLAND_IMAGE(0, "assets/island/OutsSS00.png");
        LOAD_ISLAND_IMAGE(1, "assets/island/OutsMM03.png");
        LOAD_ISLAND_IMAGE(2, "assets/island/OutsMM02.png");
        LOAD_ISLAND_IMAGE(3, "assets/island/OutsSS01.png");
        LOAD_ISLAND_IMAGE(4, "assets/island/OutsSS05.png");
        LOAD_ISLAND_IMAGE(5, "assets/island/OutsSS04.png");
        LOAD_ISLAND_IMAGE(6, "assets/island/OutsSS07.png");
        LOAD_ISLAND_IMAGE(7, "assets/island/OutsSS06.png");
#undef LOAD_ISLAND_IMAGE
    };

    auto load_water = [this]()
    {
        water.model.mesh.vbuf = sg_alloc_buffer();
        water.model.textures.reserve(8);
        water.model.mesh.bindings = (sg_bindings){
            .vertex_buffers[0] = island.model.mesh.vbuf,
            .fs = {
                .samplers[0] = sg_make_sampler({
                    .min_filter = SG_FILTER_LINEAR,
                    .mag_filter = SG_FILTER_LINEAR,
                    .wrap_u = SG_WRAP_REPEAT,
                    .wrap_v = SG_WRAP_REPEAT,
                    .label = "object-sampler",
                }),
            },
        };
        batteries::load_obj({
            .buffer_id = water.model.mesh.vbuf,
            .mesh = &water.model.mesh,
            .path = "assets/island/Sea.obj",
            .buffer = SG_RANGE(file_buffer),
        });

#define LOAD_SEA_IMAGE(i, filepath)                           \
    water.model.textures[i] = sg_alloc_image();               \
    batteries::load_img({.image_id = water.model.textures[i], \
                         .path = filepath,                    \
                         .buffer = SG_RANGE(file_buffer)})
        LOAD_SEA_IMAGE(0, "assets/island/Sea.SS00.png");
        LOAD_SEA_IMAGE(1, "assets/island/Sea.SS01.png");
        LOAD_SEA_IMAGE(2, "assets/island/Sea.SS02.png");
        LOAD_SEA_IMAGE(3, "assets/island/Sea.SS04.png");
        LOAD_SEA_IMAGE(4, "assets/island/Sea.SS05.png");
        LOAD_SEA_IMAGE(5, "assets/island/Sea.SS06.png");
        LOAD_SEA_IMAGE(6, "assets/island/Sea.SS07.png");
        LOAD_SEA_IMAGE(7, "assets/island/Sea.SS08.png");
#undef LOAD_SEA_IMAGE
    };

    ambient = (batteries::ambient_t){
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    light = (batteries::light_t){
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
    };

    camera.position = {115.0f, 75.0f, 850.0f};

    load_island();
    load_water();

    island.model.transform.scale = glm::vec3(0.05f);
    water.model.transform.scale = glm::vec3(0.05f);

    palette = pal[0];
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    static auto ry = 0.0f;
    ry += dt;

    // sugar: rotate light
    const auto rym = glm::rotate(ry, glm::vec3(0.0f, 1.0f, 0.0f));
    light.position = rym * light_orbit_radius;
}

void Scene::Render(void)
{
    const auto view_proj = camera.projection() * camera.view();

    // initialize uniform data
    const IslandPass::vs_params_t vs_island_params = {
        .view_proj = view_proj,
        .model = island.model.transform.matrix(),
    };
    const IslandPass::fs_params_t fs_island_params = {
        .ambient = ambient,
        .palette = palette,
    };

    framebuffer.RenderTo([&]()
                         {
        islandPass.Apply(vs_island_params, fs_island_params);
        island.model.Render(); });
    framebuffer.Render();
}

void Scene::Debug(void)
{
    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("%.1f, %.1f, %.1f", camera.position.x, camera.position.y, camera.position.z);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);

    if (ImGui::CollapsingHeader("Ambient"))
    {
        ImGui::SliderFloat("Intensity", &ambient.intensity, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color", &ambient.color[0]);
    }
    if (ImGui::CollapsingHeader("Light"))
    {
        ImGui::SliderFloat("Brightness", &light.brightness, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color", &light.color[0]);
    }

    ImGui::End();
}