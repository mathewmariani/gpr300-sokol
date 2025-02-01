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
  }

  void __dbgui_begin(void)
  {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(float(sapp_width()), float(sapp_height()));

    ImGui_ImplOpenGL3_NewFrame();
	  ImGui::NewFrame();
  }

  void __dbgui_end(void)
  {
    ImGui::Render();
	  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  void __dbgui_event(const sapp_event *ev)
  {
  }
}