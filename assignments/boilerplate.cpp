// sokol
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_fetch.h"

// libs
#include "imgui/imgui.h"

// sokol-imgui
#define SOKOL_IMGUI_IMPL
#include "sokol/sokol_imgui.h"

// forward declare
void init(void);
void cleanup(void);
void frame(void);
void event(const sapp_event *event);

// main entry point
sapp_desc sokol_main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  return (sapp_desc){
      .init_cb = init,
      .frame_cb = frame,
      .cleanup_cb = cleanup,
      .event_cb = event,
      .width = 800,
      .height = 600,
      .window_title = "gpr300-sokol",
      .logger = {
          .func = slog_func,
      },
  };
}

//
// Boilerplate; a defacto entry point for all assignments.
//

#include "scene.h"

static Scene *scene;

void init(void)
{
  // setup sokol-gfx
  sg_setup({
      .environment = sglue_environment(),
      .logger.func = slog_func,
  });

  // setup sokol-fetch
  sfetch_setup({
      .max_requests = 24,
      .num_channels = 1,
      .num_lanes = 1,
      .logger.func = slog_func,
  });

  // setup sokol-imgui
  simgui_setup({
      .logger.func = slog_func,
  });

  scene = new Scene();
}

void cleanup(void)
{
  delete scene;
  scene = nullptr;

  sfetch_shutdown();
  simgui_shutdown();
  sg_shutdown();
}

void frame(void)
{
  const auto t = (float)sapp_frame_duration();
  const auto w = sapp_width();
  const auto h = sapp_height();

  sfetch_dowork();
  simgui_new_frame({w, h, t, sapp_dpi_scale()});

  scene->Update(t);
  scene->Render();
  scene->Debug();

  // FIXME:
  // NOTE: this assumes scene.Render() doens't call `sg_end_pass()` and `sg_commit()`

  // draw ui
  simgui_render();

  sg_end_pass();
  sg_commit();
}

void event(const sapp_event *event)
{
  simgui_handle_event(event);
  scene->Event(event);
}