#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "glhelper.h"
#include "gdalreader.h"
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
    init_glfw(&window);
    glClearColor(0,0,0,0); // set the clear color to black;

    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);
    GDALImage image;
    puts("sampling...");
    sample(filepath , &image);
/*    float *Test = (float*)malloc(sizeof(float) * window_width * window_height * 3 );*/
    /*int i;*/
    /*for(i = 0; i < window_width*window_height; i+=3) {*/
        /*Test[i] = 0;*/
        /*Test[i+1] = 200;*/
        /*Test[i+2] = 0;*/
        /*Test[i+3] = 255;*/
    /*}*/
    // main loop
    puts("done sampling");
    while(!glfwWindowShouldClose(window))
    {
        glfwGetWindowSize(window, &window_width, &window_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(window_width, window_height, GL_R, GL_FLOAT, image.data);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    CPLFree(image.data);
    /*
     *free(Test);
     */
}
