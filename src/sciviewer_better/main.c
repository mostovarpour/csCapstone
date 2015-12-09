#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "glhelper.h"
#include "gdalreader.h"
#include "raster.h"
#ifdef win32
    #include <windows.h>
#else
    #include <pthread.h>
#endif 

#define SETFILEPATH(argc, argv) {if(argc > 1) filepath = argv[1]; }

GLFWwindow *window;
char *filepath;
int main(int argc, char **argv)
{
    SETFILEPATH(argc, argv); // check for filepath passed as command line arg
    init_glfw(&window); // initialize openGL
    /*glClearColor(0,0,0,0); // set the clear color to black;*/

    // window width, height, and the variable to hold the image we will be sampling
    int window_width, window_height;
    GDALImage image;
    GLint shader_program_id = init_shaders(); // initialize shaders
    glUseProgram(shader_program_id);
    
    puts("main loop");
    // main loop
    while(!glfwWindowShouldClose(window))
    {
        glfwGetWindowSize(window, &window_width, &window_height); // get window size
        glClear(GL_COLOR_BUFFER_BIT); // clear window
        glfwSwapBuffers(window); // swap buffers (like vsync)
        glfwPollEvents(); // glfw thing that manages window events
    }

    glfwDestroyWindow(window); // make sure the window is closed
    glfwTerminate(); // kill glfw
    if(image.data) // free the data created with CPLMalloc
        CPLFree(image.data);
    return 0;
}
