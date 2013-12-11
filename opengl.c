#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Shader source
const GLchar* vertexSource =
    "#version 150 core\n"
    "in vec2 position;"
    "void main() {"
    "   gl_Position = vec4(position, 0.0, 1.0);"
    "}";
const GLchar* fragmentSource =
    "#version 150 core\n"
    "out vec4 outColor;"
    "void main() {"
    "   outColor = vec4(1.0, 1.0, 1.0, 1.0);"
    "}";

void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(int argc, char *argv[])
{
    GLFWwindow* window;

    // Set error callback
    glfwSetErrorCallback(error_callback);

    // Initialize GLFW
    glfwInit();
    if(!glfwInit())
        exit(EXIT_FAILURE);

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(800, 600, "Cool Window", NULL, NULL);
    // Set current context to window
    glfwMakeContextCurrent(window);
    // Set key callback
    glfwSetKeyCallback(window, key_callback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    //////////////////////
    // Setup vertices
    /////////////////////

    // Vertices
    float vertices[] = {
       -0.5f,  0.5f,
        0.5f,  0.5f,
        0.5f, -0.5f,
       -0.5f, -0.5f
    };

    // Set vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate vertex buffer
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Set active buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Fill active buffer
    glBufferData(GL_ARRAY_BUFFER, 2*4*sizeof(float), vertices, GL_STATIC_DRAW);

    // Create an element array
    GLuint ebo;
    glGenBuffers(1, &ebo);

    GLuint elements[] = {
        2, 3, 0,
        0, 1, 2
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2*3*sizeof(GLuint), elements, GL_STATIC_DRAW);

    /////////////////////
    // Setup shaders
    ////////////////////

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    
    // Compile frag shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link and use program
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Specify and enable vertex attribute
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    // Event loop
    while(!glfwWindowShouldClose(window))
    {

        // Clear the screen
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
  
        // Draw square
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Draw triangle
//        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // Clean up
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
