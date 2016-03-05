#include "headers/glhelper.h"

void error_callback(int error, const char* description) {
    printf("Error %d: %s\n", error, description);
}

void init_glfw(ptGLFWwindow window, int width, int height)
{
    // set error callback
    glfwSetErrorCallback(error_callback);
    // initialize glfw, quit if failed
    if(!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
	/*
	The following is a common formula for resizing an image while keeping the aspect ratio.
	For now, the width will be a static value of 1000 pixels
	*/
	int setWidth = 1000;
	int setHeight;
	//setHeight = (height/width) * setWidth;

    // check supplied width and height
    if(width < ARGS_MINIMUM_WIDTH)
        width = 800;
    if(height < ARGS_MINIMUM_HEIGHT)
        height = 600;

    // TODO set width and height for aspect ratio here
    // in main.c there is a function call to create_gdal_image, that creates
    // a struct with data on the width and height of the image
    // You may have to move things around so you can pass that data to this function
    // which creates the window.
    
    // get screen size
    *window = glfwCreateWindow(width, height, "Gigapixel Viewer", NULL, NULL);
    if(!*window) 
    {
        glfwTerminate();
        puts("Failed to create window.");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(*window);
}
