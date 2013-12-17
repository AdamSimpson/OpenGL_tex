#ifndef MULTI_TEX_H
#define MULTI_TEX_H

#include "GLES2/gl2.h"
#include "egl_utils.h"

#define NUM_TEXTURES 2

typedef struct
{
    // OpenGL|ES state
    EGL_STATE_T egl_state;

    // Program handle
    GLuint program;

    // Locations
    GLint position_location;
    GLint tex_coord_location;
    GLint tex_location;

    // Texture handles
    GLuint textures[NUM_TEXTURES];

    // Texture attributes
    uint32_t tex_width;
    uint32_t tex_height;

    volatile int terminate;
} STATE_T;

void create_textures();

#endif
