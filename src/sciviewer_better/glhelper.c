#include "headers/glhelper.h"

void error_callback(int error, const char* description) {
    printf("Error %d: %s\n", error, description);
}

void init_glfw(ptGLFWwindow window)
{
    // set error callback
    glfwSetErrorCallback(error_callback);
    // initialize glfw, quit if failed
    if(!glfwInit())
        exit(EXIT_FAILURE);
    
    // get screen size
    *window = glfwCreateWindow(800, 600, "Gigapixel Viewer", NULL, NULL);
    if(!*window) 
    {
        glfwTerminate();
        puts("Failed to create window.");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(*window);
    puts("made window");
}
