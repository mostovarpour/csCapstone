#include "headers/glhelper.h"

void error_callback(int error, const char* description) {
    printf("Error %d: %s\n", error, description);
}

void init_glfw(ptGLFWwindow window, int winWidth, int winHeight, int picWidth, int picHeight)
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

	//One of the values has to be converted to a double in order to recieve a decimal value answer for the following division equation
	double heightDouble = picHeight;
	//This is a standard formula for resizing an image while keeping the aspect ratio. plus 50 pixels for the toolbar
	setHeight = ((heightDouble / picWidth) * setWidth) + 50;

	winWidth = setWidth;
	winHeight = setHeight;
    // TODO set width and height for aspect ratio here
    // in main.c there is a function call to create_gdal_image, that creates
    // a struct with data on the width and height of the image
    // You may have to move things around so you can pass that data to this function
    // which creates the window.
    
    // get screen size
    *window = glfwCreateWindow(winWidth, winHeight, "Gigapixel Viewer", NULL, NULL);
    if(!*window) 
    {
        glfwTerminate();
        puts("Failed to create window.");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(*window);
}
