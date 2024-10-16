#include "scene.h"
#include "skybox.h"

#include "sokol/sokol_glue.h"

namespace batteries
{
  Scene::Scene()
  {
    sg_image_desc img_desc = {
        .render_target = true,
        .width = 800,
        .height = 600,
    };

    // color attachment
    img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    img_desc.label = "framebuffer-color-image";
    auto color = sg_make_image(img_desc);

    // depth attachment
    img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    img_desc.label = "framebuffer-depth-image";
    auto depth = sg_make_image(img_desc);

    pass = (sg_pass){
        .action = (sg_pass_action){
            .colors[0] = {
                .clear_value = {0.0f, 0.0f, 0.0f, 1.0f},
                .load_action = SG_LOADACTION_CLEAR,
            },
            .depth = {
                .load_action = SG_LOADACTION_CLEAR,
                .store_action = SG_STOREACTION_STORE,
                .clear_value = 1.0f,
            },
        },
        // .attachments = sg_make_attachments({
        //     .colors[0].image = color,
        //     .depth_stencil.image = depth,
        //     .label = "framebuffer-attachments",
        // }),
        .swapchain = sglue_swapchain(),
    };

    time = {
        .frame = 0.0,
        .absolute = 0.0,
        .factor = 1.0f,
        .paused = false,
    };
  }

  Scene::~Scene()
  {
  }

  void Scene::Update(float dt)
  {
    // update internal timer
    time.frame = dt;
    if (!time.paused)
    {
      time.absolute += time.frame * time.factor;
    }

    // update camera
    camera_controller.update(&camera, dt);
  }

  void Scene::Event(const sapp_event *event)
  {
    camera_controller.event(event);
  }
}