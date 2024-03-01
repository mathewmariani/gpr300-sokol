#define SOKOL_IMPL

// void glCheckError_(const char *file, int line);
// #define _SG_GL_CHECK_ERROR()           \
//   {                                    \
//     glCheckError_(__FILE__, __LINE__); \
//   }

/* headers */
#include "sokol_app.h"
#include "sokol_fetch.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"
#include "sokol_time.h"
#include "sokol_shape.h"

// opengl error checking
#include <stdio.h>
void glCheckError_(const char *file, int line)
{
  GLenum code = GL_NO_ERROR;
  while ((code = glGetError()) != GL_NO_ERROR)
  {
    const char *error;
    switch (code)
    {
    case GL_INVALID_ENUM:
      error = "INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error = "INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      error = "INVALID_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      error = "OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = "INVALID_FRAMEBUFFER_OPERATION";
      break;
    }
    fprintf(stderr, "OpenGL Error %s at %s:%d\n", error, file, line);
  }
}
