#pragma once

// opengl
#if defined(SOKOL_GLCORE)
    #if defined(__APPLE__)
        #include <OpenGL/gl3.h>
        #include <OpenGL/gl3ext.h>
    #elif defined(__linux__) || defined(__unix__)
        #define GL_GLEXT_PROTOTYPES
        #include <GL/gl.h>
    #else
        #error("opengl.h: Unknown platform")
    #endif        
#elif defined(SOKOL_GLES3)
    #include <GLES3/gl3.h>
#else
    #error("opengl.h: unknown 3D API selected; must be SOKOL_GLCORE or SOKOL_GLES3")
#endif