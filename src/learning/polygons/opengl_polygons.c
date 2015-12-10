#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "glhelper.h"

#ifdef win32
    #include <windows.h>
#else
    #include <pthread.h>
#endif 

const char *vertex_shader = 
"#version 150\n" // declare version 1.5
"in float color;" // input color
"in vec2 position;" // input is a 2d vector (X,Y)
"out float Color;" //output color to fragment shader
"void main()"
"{"
    "Color = color;"
    "gl_Position = vec4(position.x, -position.y, 0.0, 1.0);" // convert vec2 to vec4 and set it gl_position
"}";

const char *fragment_shader =
"#version 150\n"
"in float Color;"
"out vec4 outColor;" // output a vec4 (r,g,b,a)
"void main()"
"{"
    "outColor = vec4(Color, Color, Color, 1.0);" // all white
"}"
;

GLFWwindow *window;
int main()
{
    init_glfw(&window); // initialize openGL
    glewExperimental = GL_TRUE;
    glewInit(); // gets cool functions like glGenVertexArrays and glBindBuffer

    // create vertices simple triangle
    float vertices[] = {
         0.0f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 1.0f
    };

    // create vertex array object for storing shader and attribute data
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // create vertex buffer
    GLuint vbo;
    glGenBuffers(1, &vbo); // generate one buffer
    // make this buffer the "active" buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // put vertices into the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // element buffer
    GLuint elements[] = {
        0, 1, 2
    };
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // create vertex shader
    GLuint v_shader = glCreateShader(GL_VERTEX_SHADER);
    // load shader source into GPU
    glShaderSource(v_shader, 1, &vertex_shader, NULL);
    // compile the shader
    glCompileShader(v_shader);
    // check if shader compiled correctly
    GLint status;
    glGetShaderiv(v_shader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        puts("Error compiling vertex shader");
        // get error log from shader
        char buffer[512];
        glGetShaderInfoLog(v_shader, 512, NULL, buffer);
        printf("%s\n", buffer); 
    }
    // same thing for fragment shader
    GLuint f_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f_shader, 1, &fragment_shader, NULL);
    glCompileShader(f_shader);
    glGetShaderiv(f_shader, GL_COMPILE_STATUS, &status);

    if(status != GL_TRUE)
    {
        puts("Error compiling fragment shader");
        // get error log from shader
        char buffer[512];
        glGetShaderInfoLog(v_shader, 512, NULL, buffer);
        printf("%s\n", buffer); 
    }

    // combine shaders into a program
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, v_shader);
    glAttachShader(shader_program, f_shader);
    /*glBindFragDataLocation(shader_program, 0, "outColor");*/
    // link the program
    glLinkProgram(shader_program);
    // tell opengl to use this program
    glUseProgram(shader_program);

    // get position of the input position in vertex_shader
    GLint posAttribute = glGetAttribLocation(shader_program, "position"); // always 0 since it's the first and only arg
    // (arg_position, number of values for that arg, type for each component, 
    // should they be normalized?, distance (in bytes) between each position attribute,
    // offset from start of array for first attribute value)
    glVertexAttribPointer(posAttribute, 2, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
    // enable attribute array
    glEnableVertexAttribArray(posAttribute);
    GLint colorAttribute = glGetAttribLocation(shader_program, "color");
    glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(colorAttribute);

    glClearColor(0,0,0,1);
    // main loop
    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window); // swap buffers (like vsync)
        glfwPollEvents(); // glfw thing that manages window events
    }
    glfwDestroyWindow(window); // make sure the window is closed
    // cleanup
    glDeleteProgram(shader_program);
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);
    glDeleteBuffers(1, &vbo);
    glfwTerminate(); // kill glfw
    return 0;
}
