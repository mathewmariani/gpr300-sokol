/* sokol */
#include "sokol/sokol_app.h"

/* imgui */
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"

extern "C"
{
  void __dbgui_setup(void)
  {
    ImGui::CreateContext();
	  ImGui_ImplOpenGL3_Init();
  }

  void __dbgui_shutdown(void)
  {
    ImGui::DestroyContext();
  }

  void __dbgui_begin(void)
  {
    ImGuiIO* io = &ImGui::GetIO();
    io->DeltaTime = sapp_frame_duration();
    io->DisplaySize = ImVec2(float(sapp_width()), float(sapp_height()));

    ImGui_ImplOpenGL3_NewFrame();
	  ImGui::NewFrame();
  }

  void __dbgui_end(void)
  {
    ImGui::Render();
	  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  bool __dbgui_event(const sapp_event *ev)
  {
    ImGuiIO* io = &ImGui::GetIO();
    switch (ev->type) {
        case SAPP_EVENTTYPE_FOCUSED:
            break;
        case SAPP_EVENTTYPE_UNFOCUSED:
            break;
        case SAPP_EVENTTYPE_MOUSE_DOWN:
        case SAPP_EVENTTYPE_MOUSE_UP:
            io->MouseDown[ev->mouse_button] = (ev->type == SAPP_EVENTTYPE_MOUSE_DOWN);
            break;
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            io->AddMouseSourceEvent(ImGuiMouseSource_Mouse);
            io->AddMousePosEvent(ev->mouse_x, ev->mouse_y);
            break;
        case SAPP_EVENTTYPE_MOUSE_ENTER:
        case SAPP_EVENTTYPE_MOUSE_LEAVE:
            break;
        case SAPP_EVENTTYPE_MOUSE_SCROLL:
            io->MouseWheel += ev->scroll_y;
            io->MouseWheelH += ev->scroll_x;
            break;
        case SAPP_EVENTTYPE_TOUCHES_BEGAN:
            break;
        case SAPP_EVENTTYPE_TOUCHES_MOVED:
            break;
        case SAPP_EVENTTYPE_TOUCHES_ENDED:
            break;
        case SAPP_EVENTTYPE_TOUCHES_CANCELLED:
            break;
        case SAPP_EVENTTYPE_KEY_DOWN:
            break;
        case SAPP_EVENTTYPE_KEY_UP:
            break;
        case SAPP_EVENTTYPE_CHAR:
            break;
        case SAPP_EVENTTYPE_CLIPBOARD_PASTED:
            break;
        default:
            break;
    }
    return (io->WantCaptureKeyboard || io->WantCaptureMouse);
  }
}