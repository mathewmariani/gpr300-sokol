#pragma once
// #if defined(USE_DBG_UI)
#include "sokol/sokol_app.h"
#if defined(__cplusplus)
extern "C"
{
#endif
  extern void __dbgui_setup(void);
  extern void __dbgui_shutdown(void);
  extern void __dbgui_begin(void);
  extern void __dbgui_end(void);
  extern bool __dbgui_event(const sapp_event *e);
#if defined(__cplusplus)
} // extern "C"
#endif
// #else
// static inline void __dbgui_setup(void) {}
// static inline void __dbgui_shutdown(void) {}
// static inline void __dbgui_begin(void) {}
// static inline void __dbgui_end(void) {}
// static inline void __dbgui_event(const void *e) { (void)(e); }
// #endif