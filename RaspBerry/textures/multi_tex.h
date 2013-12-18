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
    GLsizei tex_width;
    GLsizei tex_height;

    int terminate;
} STATE_T;

void create_textures(STATE_T *state);
void create_vertices();
void create_shaders(STATE_T *state);
void draw_textures(STATE_T *state);
void update_texture_row(STATE_T *state, GLuint texture, GLenum tex_unit, GLsizei row, GLubyte *row_pixels);

#endif
