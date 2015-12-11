#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "glhelper.h"
#include "gdalreader.h"

#ifdef win32
    #include <windows.h>
#else
    #include <pthread.h>
#endif 

const char *vertex_shader = 
"#version 150\n" // declare version 1.5
"in vec2 texCoord;"
"in vec2 position;" // input is a 2d vector (X,Y)
"out vec2 TexCoord;"
"void main()"
"{"
    "TexCoord = texCoord;"
    "gl_Position = vec4(position, 0.0, 1.0);" // convert vec2 to vec4 and set it gl_position
"}";

const char *fragment_shader =
"#version 150\n"
"uniform sampler2D tex;"
"in vec2 TexCoord;"
"out vec4 outColor;" // output a vec4 (r,g,b,a)
"void main()"
"{"
    "vec4 tmp = texture2D(tex, TexCoord);"
    "outColor = vec4(tmp.r, tmp.r, tmp.r, 1.0f);"
"}"
;

// pass in pointers so we can free the memory in the gpu later
void setup_polygons(GLuint *vao, GLuint *ebo, GLuint *vbo, GLuint *v_shader, GLuint *f_shader, GLuint *shader_program) 
{
    glewInit();
    // create vertices simple triangle
    float vertices[] = {
        //position     //texture coordinates
        -1.0f,  1.0f,  0.0f, 0.0f, // top left 
         1.0f,  1.0f,  1.0f, 0.0f, // top right
         1.0f, -1.0f,  1.0f, 1.0f, // bottom-right
        -1.0f, -1.0f,  0.0f, 1.0f  // bottom-left
    };

    // create vertex array object for storing shader and attribute data
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);


    // create vertex buffer
    glGenBuffers(1, vbo); // generate one buffer
    // make this buffer the "active" buffer
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    // put vertices into the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // create element buffer
    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };
    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    // create vertex shader
    *v_shader = glCreateShader(GL_VERTEX_SHADER);
    // load shader source into GPU
    glShaderSource(*v_shader, 1, &vertex_shader, NULL);
    // compile the shader
    glCompileShader(*v_shader);
    // check if shader compiled correctly
    GLint status;
    glGetShaderiv(*v_shader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        puts("Error compiling vertex shader");
        // get error log from shader
        char buffer[512];
        glGetShaderInfoLog(*v_shader, 512, NULL, buffer);
        printf("%s\n", buffer); 
    }
    // same thing for fragment shader
    *f_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(*f_shader, 1, &fragment_shader, NULL);
    glCompileShader(*f_shader);
    glGetShaderiv(*f_shader, GL_COMPILE_STATUS, &status);

    if(status != GL_TRUE)
    {
        puts("Error compiling fragment shader");
        // get error log from shader
        char buffer[512];
        glGetShaderInfoLog(*f_shader, 512, NULL, buffer);
        printf("%s\n", buffer); 
    }
    // combine shaders into a program
    *shader_program = glCreateProgram();
    glAttachShader(*shader_program, *v_shader);
    glAttachShader(*shader_program, *f_shader);
    /*glBindFragDataLocation(*shader_program, 0, "outColor");*/
    // link the program
    glLinkProgram(*shader_program);
    // tell opengl to use this program
    glUseProgram(*shader_program);

    // get position of the input position in vertex_shader
    GLint posAttribute = glGetAttribLocation(*shader_program, "position"); // always 0 since it's the first and only arg
    // (arg_position, number of values for that arg, type for each component, 
    // should they be normalized?, distance (in bytes) between each position attribute,
    // offset from start of array for first attribute value)
    glVertexAttribPointer(posAttribute, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    // enable attribute array
    glEnableVertexAttribArray(posAttribute);
}

GLFWwindow *window;
int main(int argc, char** argv)
{
    init_glfw(&window); // initialize openGL
    // In my case this isn't actually necessary which is kind of weird, but so it works
    // on all other machines it should be kept. I don't think windows is smart enough
    // to get those functions on its own even though linux may be
    glewExperimental = GL_TRUE;
    glewInit(); // gets cool functions like glGenVertexArrays and glBindBuffer
    GLuint vao, ebo, vbo, v_shader, f_shader, shader_program;
    setup_polygons(&vao, &ebo, &vbo, &v_shader, &f_shader, &shader_program);

    /*******************
     *TEXTURE STUFF YAY*
     *******************/
    GLuint tex;
    glGenTextures(1, &tex); // generate one texture beginning at &tex
    glBindTexture(GL_TEXTURE_2D, tex); // set this texture as the current texture

    // setup texture for use in shaders
    GLint texAttribute = glGetAttribLocation(shader_program, "texCoord");
    glVertexAttribPointer(texAttribute, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    float color[] = { 1.0f, 0.0f, 1.0f, 0.8f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);


    int width, height;
    glfwGetWindowSize(window, &width, &height);
    /*********************************
     *GET TEXTURE FROM GDAL IMAGE
     *********************************/
    GDALImage image;
    sample(argv[1], &image, width, height);
    int i, j;
    long max = width * height;
    long index;
    /*
     *for(i = 0; i < max; i++)
     *{
     *    image.data[i] = image.data[i]/255.0f;
     *}
     */
    // actually load a texture!
    float pixels[] = {
        1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 
        1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 
        1, 0, 1, 0, 1, 1, 1, 0, 0, 0,
        1, 1, 0, 0, 1, 0, 1, 0, 0, 0
    };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // generate mip map
    glGenerateMipmap(GL_TEXTURE_2D);
    GLint texAttrib = glGetAttribLocation(shader_program, "texCoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(texAttrib);

    // main loop
    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window); // swap buffers (like vsync)
        glfwPollEvents(); // glfw thing that manages window events
    }
    glfwDestroyWindow(window); // make sure the window is closed
    // cleanup
    CPLFree(image.data);
    glDeleteTextures(1, &tex);
    glDeleteProgram(shader_program);
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);
    glDeleteBuffers(1, &vbo);
    glfwTerminate(); // kill glfw
    return 0;
}
