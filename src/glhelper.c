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


//These variables control the position of the image in the window. + will move it right, - will move it left
double posX = 0.0;
double posY = 0.0;

double zoomFactor = 1.0;

void glZoom(ptGLFWwindow window, int picWidth, int picHeight) {

	int winWidth;
	int winHeight;

	int setWidth = 1000;
	int setHeight;

	//One of the values has to be converted to a double in order to recieve a decimal value answer for the following division equation
	double heightDouble = picHeight;
	//This is a standard formula for resizing an image while keeping the aspect ratio. plus 50 pixels for the toolbar
	setHeight = ((heightDouble / picWidth) * setWidth) + 50;

	winWidth = setWidth;
	winHeight = setHeight;

	//GLFW_RELEASE will be true if the key IS released so they are all true if you don't touch the keyboard
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
		zoomFactor = zoomFactor - 0.0005;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
		zoomFactor = zoomFactor + 0.0005;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
		posX = posX + 0.2;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
		posX = posX - 0.2;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE) {
		posY = posY + 0.2;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
		posY = posY - 0.2;
	}

	glViewport(posX, posY, winWidth*zoomFactor, winHeight*zoomFactor);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60.0, 1, 1.5, 20);
	//glMatrixMode(GL_MODELVIEW);

}