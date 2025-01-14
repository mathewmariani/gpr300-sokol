/* sokol */
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_app.h"
#include "sokol/sokol_glue.h"

/* imgui */
#include "imgui/imgui.h"

#define SOKOL_IMGUI_IMPL
#include "sokol/sokol_imgui.h"

extern "C"
{
  namespace
  {
    sg_pass pass;
  }
  void __dbgui_setup(void)
  {
    simgui_setup((simgui_desc_t){0});
    pass = (sg_pass){
        .action = {
            .colors = {{.load_action = SG_LOADACTION_DONTCARE}},
        },
        .swapchain = sglue_swapchain(),
    };
  }

  void __dbgui_shutdown(void)
  {
    simgui_shutdown();
  }

  void __dbgui_begin(void)
  {
    simgui_new_frame((simgui_frame_desc_t){
        .width = sapp_width(),
        .height = sapp_height(),
        .delta_time = sapp_frame_duration(),
        .dpi_scale = sapp_dpi_scale(),
    });
  }

  void __dbgui_end(void)
  {
    sg_begin_pass(&pass);
    simgui_render();
    sg_end_pass();
    sg_commit();
  }

  void __dbgui_event(const sapp_event *ev)
  {
    simgui_handle_event(ev);
    if (ImGui::GetIO().WantCaptureMouse && (ev->type == SAPP_EVENTTYPE_MOUSE_DOWN || ev->type == SAPP_EVENTTYPE_MOUSE_MOVE))
    {
      return;
    }
  }
}