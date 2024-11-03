#include "scene.h"
#include "imgui/imgui.h"

static struct
{
    float angle;
    glm::quat yRotationQuaternion;
    glm::quat xRotationQuaternion;
} state;

Scene::Scene()
{
    suzanne.Load("assets/suzanne.obj");
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    state.angle += (time.frame * time.factor) / 1000 / 6 * 2 * 3.14159f;

    // angle = time.absolute
    state.yRotationQuaternion = glm::angleAxis(state.angle, glm::vec3(0.0f, 1.0f, 0.0f));
    state.xRotationQuaternion = glm::angleAxis(state.angle, glm::vec3(1.0f, 0.0f, 0.0f));

    suzanne.transform.rotation *= state.yRotationQuaternion;
    suzanne.transform.rotation *= state.xRotationQuaternion;
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    // initialize uniform data
    const BlinnPhong::vs_params_t vs_blinnphong_params = {
        .view_proj = view_proj,
        .model = suzanne.transform.matrix(),
    };

    // render to framebuffer
    sg_begin_pass(&framebuffer.pass);
    sg_apply_pipeline(blinnphong.pipeline);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(vs_blinnphong_params));
    if (suzanne.loaded)
    {
        sg_apply_bindings({
            .vertex_buffers[0] = suzanne.mesh.vertex_buffer,
            .index_buffer = suzanne.mesh.index_buffer,
        });
        sg_draw(0, suzanne.mesh.indices.size(), 1);
    }
    sg_end_pass();

    // render framebuffer
    sg_begin_pass(&pass);
    sg_apply_pipeline(framebuffer.pipeline);
    sg_apply_bindings(&framebuffer.bindings);
    sg_draw(0, 6, 1);
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 1.0f);

    if (ImGui::CollapsingHeader("Camera"))
    {
        ImGui::Text("Position: %.2f, %.2f, %.2f", camera.position[0], camera.position[1], camera.position[2]);
    }

    ImGui::End();
}