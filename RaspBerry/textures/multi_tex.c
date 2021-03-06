
#include <stdio.h>
#include <assert.h>

#include "multi_tex.h"
#include "egl_utils.h"

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "linux/input.h"

#include "bcm_host.h"

void create_textures(STATE_T *state)
{
    int i,j;

    state->tex_width = 800;
    state->tex_height = 1080;

    // First image
    GLubyte *pixels = malloc(state->tex_width*state->tex_height*sizeof(GLubyte));
    for(i=0; i<state->tex_height; i++) {
        for(j=0; j<state->tex_width; j++) {
            pixels[i*state->tex_width + j] = 0;
        }
    }
      
    // Pixel packing
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Generate texture
    glGenTextures(NUM_TEXTURES, state->textures);

    // Set texture unit 0 and bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->textures[0]);

    // Load texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, state->tex_width, state->tex_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);

    // Free pixels
    free(pixels);

    // Set filtering modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Second image
    GLubyte *pixels2 = malloc(state->tex_width*state->tex_height*sizeof(GLubyte));
    for(i=0; i<state->tex_height; i++) {
        for(j=0; j<state->tex_width; j++) {
            pixels2[i*state->tex_width + j] = 255;
        }
    }
 
    // Set texture unit 1 and bind texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, state->textures[1]);

    // Load texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, state->tex_width, state->tex_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels2);

    // Free pixels
    free(pixels2);

    // Set filtering modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

void update_texture_row(STATE_T *state, GLuint texture, GLenum tex_unit, GLsizei row, GLubyte *row_pixels)
{
    glActiveTexture(tex_unit);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row, state->tex_width, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE, row_pixels);
}

void update_texture_rows(STATE_T *state, GLuint texture, GLenum tex_unit, GLsizei row, GLubyte *row_pixels)
{
    glActiveTexture(tex_unit);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row, state->tex_width, 10, GL_LUMINANCE, GL_UNSIGNED_BYTE, row_pixels);
}

void create_vertices()
{
    // Vertices: Pos(x,y) Tex(x,y)
    float vertices[] = {
        // Image 0 vertices
        -1.0f,   1.0f, 0.0f, 0.0f, // Top left
        -0.005f, 1.0f, 1.0f, 0.0f, // Top right
        -0.005f,-1.0f, 1.0f, 1.0f, // Bottom right
	-1.0f,  -1.0f, 0.0f, 1.0f,  // Bottom left
        // Image 1 vertices
         0.005f, 1.0f, 0.0f, 0.0f, // Top left
         1.0f,   1.0f, 1.0f, 0.0f, // Top right
         1.0f,  -1.0f, 1.0f, 1.0f, // Bottom right
	 0.005f,-1.0f, 0.0f, 1.0f  // Bottom left
    };

    // Generate vertex buffer
    GLuint vbo;
    glGenBuffers(1, &vbo);
    // Set buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Fill buffer
    glBufferData(GL_ARRAY_BUFFER, NUM_TEXTURES*4*4*sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    // Elements
    GLubyte elements[] = {
        2, 3, 0,
        0, 1, 2
    };
    // Generate element buffer
    GLuint ebo;
    glGenBuffers(1, &ebo);
    // Set buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // Fill buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2*3*sizeof(GLubyte), elements, GL_STATIC_DRAW);

}

void create_shaders(STATE_T *state)
{
    // Shader source
    const GLchar* vertexSource =
        "attribute vec2 position;"
        "attribute vec2 tex_coord;"
        "varying vec2 frag_tex_coord;"
        "void main() {"
        "   gl_Position = vec4(position, 0.0, 1.0);"
        "   frag_tex_coord = tex_coord;"
        "}";
    const GLchar* fragmentSource =
        "precision mediump float;"
        "varying vec2 frag_tex_coord;"
        "uniform sampler2D tex;"
        "void main() {"
        "   gl_FragColor = texture2D(tex, frag_tex_coord);"
        "}";

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    showlog(vertexShader);   

    // Compile frag shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    showlog(fragmentShader);

    // Create shader program
    state->program = glCreateProgram();
    glAttachShader(state->program, vertexShader);
    glAttachShader(state->program, fragmentShader);
   
    // Link and use program
    glLinkProgram(state->program);
    glUseProgram(state->program);
    check();

    // Get position location
    state->position_location = glGetAttribLocation(state->program, "position");
    // Get tex_coord location
    state->tex_coord_location = glGetAttribLocation(state->program, "tex_coord");
    // Get tex uniform location
    state->tex_location = glGetUniformLocation(state->program, "tex");
}

void draw_textures(STATE_T *state)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Size of each vertex in bytes
    size_t vert_size = 4*sizeof(GL_FLOAT);

    // Draw image 0
    glVertexAttribPointer(state->position_location, 2, GL_FLOAT, GL_FALSE, vert_size, 0);
    glEnableVertexAttribArray(state->position_location);
    glVertexAttribPointer(state->tex_coord_location, 2, GL_FLOAT, GL_FALSE, vert_size,(void*)(2*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(state->tex_coord_location);
    glUniform1i(state->tex_location, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

    // Draw image 1
    glVertexAttribPointer(state->position_location, 2, GL_FLOAT, GL_FALSE, vert_size, (void*)(4*vert_size));
    glEnableVertexAttribArray(state->position_location);
    glVertexAttribPointer(state->tex_coord_location, 2, GL_FLOAT, GL_FALSE, vert_size,(void*)(4*vert_size+2*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(state->tex_coord_location);
    glUniform1i(state->tex_location, 1);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
}

int main(int argc, char *argv[])
{
    // Setup initial state
    STATE_T state;
    memset(&state, 0, sizeof(STATE_T));

    bcm_host_init();
      
    // Start OGLES
    init_ogl(&state.egl_state);

    // Create and set textures
    create_textures(&state);

    // Create and set vertices
    create_vertices();

    // Create and set shaders
    create_shaders(&state);

    //////////////////////////////
    // Testing only
    ////////////////////////////////
    int i = 0;
    GLubyte *row = malloc(state.tex_width*sizeof(GLubyte));
    memset(row, 0, state.tex_width*sizeof(GLubyte));
    GLubyte *row2 = malloc(10*state.tex_width*sizeof(GLubyte));
    memset(row2, 255, 10*state.tex_width*sizeof(GLubyte)); 

    // Event loop
    while(!state.terminate)
    {
       ///////////////////////////
       // Testing only
       ///////////////////////
	glFlush();
       if(i < state.tex_height) {
        // Testing row update
        update_texture_row(&state, state.textures[1], GL_TEXTURE1, i, row);
        update_texture_rows(&state, state.textures[0], GL_TEXTURE0, i*10, row2);
        i++;
        }

	// Draw textures
	draw_textures(&state);

        // Swap buffers
        egl_swap(&state.egl_state);

        // Check for keyboard input
	int key_press = get_key_press(&state.egl_state);	
	if(key_press == KEY_Q)
	    state.terminate=1;
    }


    // Tidy up
    exit_func(&state.egl_state);

    return 0;
}
