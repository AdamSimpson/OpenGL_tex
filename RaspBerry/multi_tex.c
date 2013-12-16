#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "bcm_host.h"

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

#define num_textures 2

typedef struct
{
    uint32_t screen_width;
    uint32_t screen_height;

    // OpenGL|ES objects
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    // Program handle
    GLuint program;

    // Locations
    GLint position_location;
    GLint tex_coord_location;
    GLint texture_location;

    // Texture handles
    GLuint textures[num_textures];


} STATE_T;

static void init_ogl(STATE_T *state);
static void exit_func(STATE_T *state);
void create_textures();

static volatile int terminate;

#define check() assert(glGetError() == 0)
static void showlog(GLint shader)
{
   // Prints the compile log for a shader
   char log[1024];
   glGetShaderInfoLog(shader,sizeof log,NULL,log);
   printf("%d:shader:\n%s\n", shader, log);
}

// Description: Sets the display, OpenGL|ES context and screen stuff
static void init_ogl(STATE_T *state)
{
   int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };
   
   static const EGLint context_attributes[] =
   {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };
   EGLConfig config;

   // get an EGL display connection
   state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(state->display!=EGL_NO_DISPLAY);

   // initialize the EGL display connection
   result = eglInitialize(state->display, NULL, NULL);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglBindAPI(EGL_OPENGL_ES_API);
   assert(EGL_FALSE != result);

   // create an EGL rendering context
   state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
   assert(state->context!=EGL_NO_CONTEXT);

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
   assert( success >= 0 );

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = state->screen_width;
   dst_rect.height = state->screen_height;
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = state->screen_width << 16;
   src_rect.height = state->screen_height << 16;

   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );
         
   dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);
      
   nativewindow.element = dispman_element;
   nativewindow.width = state->screen_width;
   nativewindow.height = state->screen_height;
   vc_dispmanx_update_submit_sync( dispman_update );
      
   state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
   assert(state->surface != EGL_NO_SURFACE);

   // connect the context to the surface
   result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
   assert(EGL_FALSE != result);

   // Set background color and clear buffers
   glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
   glClear( GL_COLOR_BUFFER_BIT );

}

static void exit_func(STATE_T *state)
// Function to be passed to atexit().
{
   // clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(state->display, state->surface);

   // Release OpenGL resources
   eglMakeCurrent( state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroySurface( state->display, state->surface );
   eglDestroyContext( state->display, state->context );
   eglTerminate( state->display );

   printf("close\n");
} // exit_func()

void create_textures(STATE_T *state)
{
    // First image
    GLubyte pixels[] =
    {
        255,   0,   0,
          0, 255,   0,
	  0,   0, 255,
	255, 255,   0
    };
   
    // Pixel packing
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Generate texture
    glGenTextures(num_textures, state->textures);

    // Set texture unit 0 and bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->textures[0]);

    // Load texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Set filtering modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Second image
    GLubyte pixels2[] =
    {
        255, 255,    0,
          0, 255,   125,
	125,   0  , 255,
	255,   0,   0
    };
 
    // Set texture unit 1 and bind texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, state->textures[1]);

    // Load texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels2);

     // Set filtering modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

int main(int argc, char *argv[])
{
    STATE_T state;

    bcm_host_init();
      
    // Start OGLES
    init_ogl(&state);

    // Create and set texture
    create_textures(&state);

    //////////////////////
    // Setup vertices
    /////////////////////

    // Vertices: Pos(x,y) Tex(x,y)
    float vertices[] = {
        // Image 0 vertices
        -0.5f,  0.5f, 0.0f, 0.0f, // Top left
         0.5f,  0.5f, 1.0f, 0.0f, // Top right
         0.5f, -0.5f, 1.0f, 1.0f, // Bottom right
	-0.5f, -0.5f, 0.0f, 1.0f,  // Bottom left
        // Image 1 vertices
        -0.5f,  0.5f, 0.0f, 0.0f, // Top left
         0.5f,  0.5f, 1.0f, 0.0f, // Top right
         0.5f, -0.5f, 1.0f, 1.0f, // Bottom right
	-0.5f, -0.5f, 0.0f, 1.0f  // Bottom left
    };

    // Generate vertex buffer
    GLuint vbo;
    glGenBuffers(1, &vbo);
    // Set buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Fill buffer
    glBufferData(GL_ARRAY_BUFFER, num_textures*4*4*sizeof(GLfloat), vertices, GL_STATIC_DRAW);

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

    /////////////////////
    // Setup shaders
    ////////////////////

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
    state.program = glCreateProgram();
    glAttachShader(state.program, vertexShader);
    glAttachShader(state.program, fragmentShader);
   
    // Link and use program
    glLinkProgram(state.program);
    glUseProgram(state.program);
    check();

    // Get position location
    state.position_location = glGetAttribLocation(state.program, "position");
    // Get tex_coord location
    state.tex_coord_location = glGetAttribLocation(state.program, "tex_coord");
    // Get texture uniform location
    state.texture_location = glGetUniformLocation(state.program, "tex");


   // Clear the screen
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Event loop
    while(!terminate)
    {
        // Draw image 0
        glVertexAttribPointer(state.position_location, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), 0);
        glEnableVertexAttribArray(state.position_location);
        glVertexAttribPointer(state.tex_coord_location, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT),(void*)(2*sizeof(GL_FLOAT)));
        glEnableVertexAttribArray(state.tex_coord_location);
        glUniform1i(state.texture_location, 1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

        // Draw image 1
        glVertexAttribPointer(state.position_location, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT), (void*)(4*4*sizeof(GL_FLOAT)));
        glEnableVertexAttribArray(state.position_location);
        glVertexAttribPointer(state.tex_coord_location, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GL_FLOAT),(void*)(4*4*sizeof(GL_FLOAT)+2*sizeof(GL_FLOAT)));
        glEnableVertexAttribArray(state.tex_coord_location);
        glUniform1i(state.texture_location, 1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

        // Swap buffers
        eglSwapBuffers(state.display, state.surface);

    }

    // Tidy up
    exit_func(&state);

    return 0;
}
