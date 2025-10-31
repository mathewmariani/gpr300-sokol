#pragma once

// opengl
#if defined(SOKOL_GLCORE)
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#elif defined(SOKOL_GLES3)
#include <GLES3/gl3.h>
#else
#error "You must define SOKOL_GLCORE or SOKOL_GLES3 before including opengl.h"
#endif