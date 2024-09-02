#ifndef BOILERPLATE_INCLUDED
#define BOILERPLATE_INCLUDED

// sokol
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_time.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_shape.h"

// libs
#include "imgui/imgui.h"

namespace boilerplate
{
  constexpr int kilobytes(int n) { return 1024 * n; }
  constexpr int megabytes(int n) { return 1024 * kilobytes(n); }
  constexpr int gigabytes(int n) { return 1024 * megabytes(n); }

  void setup(void);
  void shutdown(void);
  void frame(void);
  void event(const sapp_event *event);
}

#endif // BOILERPLATE_INCLUDED
#ifdef BOILERPLATE_IMPL

// sokol-fetch
#include "sokol/sokol_fetch.h"

// sokol-imgui
#define SOKOL_IMGUI_IMPL
#include "sokol/sokol_imgui.h"

// libs
#include "stb/stb_image.h"

namespace boilerplate
{
  void setup(void)
  {
    // setup sokol-gfx
    sg_setup((sg_desc){
        .environment = sglue_environment(),
        .logger.func = slog_func,
    });

    // setup sokol-time
    stm_setup();

    // setup sokol-fetch
    sfetch_setup((sfetch_desc_t){
        .max_requests = 24,
        .num_channels = 1,
        .num_lanes = 1,
        .logger.func = slog_func,
    });

    // setup sokol-imgui
    simgui_setup((simgui_desc_t){
        .logger.func = slog_func,
    });

    // stbi_set_flip_vertically_on_load(true);
  }

  void shutdown(void)
  {
    sfetch_shutdown();
    simgui_shutdown();
    sg_shutdown();
  }

  void frame(void)
  {
    sfetch_dowork();
    simgui_new_frame({sapp_width(), sapp_height(), sapp_frame_duration(), sapp_dpi_scale()});
  }

  void event(const sapp_event *event)
  {
    simgui_handle_event(event);
  }
}

// forward declarations
static void init(void);
static void frame(void);
static void event(const sapp_event *event);
static void cleanup(void);

// main entry point
sapp_desc sokol_main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  return (sapp_desc){
      .init_cb = init,
      .frame_cb = frame,
      .event_cb = event,
      .cleanup_cb = cleanup,
      .width = 800,
      .height = 800,
      .window_title = "gpr300-sokol",
      .logger.func = slog_func,
  };
}

#endif // BOILERPLATE_IMPL