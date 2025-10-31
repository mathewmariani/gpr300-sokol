// sokol
#include "sokol/sokol_app.h"
#include "sokol/sokol_log.h"

// libs
#include "dbgui/dbgui.h"
#include "stb/stb_image.h"

// batteries
#include "batteries/opengl.h"

// forward declare
void init(void);
void cleanup(void);
void frame(void);
void event(const sapp_event* event);

// main entry point
sapp_desc sokol_main(int argc, char* argv[])
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
#if defined(SOKOL_GLCORE)
#if defined(__APPLE__)
        .gl_major_version = 4,
        .gl_minor_version = 1,
#else
        .gl_major_version = 4,
        .gl_minor_version = 2,
#endif
#endif
    };
}

//
// Boilerplate; a defacto entry point for all assignments.
//

#include "scene.h"

static Scene* scene;

void init(void)
{
    glViewport(0, 0, sapp_width(), sapp_height());

    __dbgui_setup();

    stbi_set_flip_vertically_on_load(true);

    scene = new Scene();
}

void cleanup(void)
{
    delete scene;
    scene = nullptr;

    __dbgui_shutdown();
}

void frame(void)
{
    const auto t = (float)sapp_frame_duration();
    const auto w = sapp_width();
    const auto h = sapp_height();

    scene->Update(t);
    scene->Render();

    // draw ui
    __dbgui_begin();
    scene->Debug();
    __dbgui_end();
}

void event(const sapp_event* event)
{
    if (__dbgui_event(event))
    {
        return;
    }
    scene->Event(event);
}