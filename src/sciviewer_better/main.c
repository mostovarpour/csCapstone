#include "os.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "glhelper.h"
#include "gdalreader.h"
#include "raster.h"
GLFWwindow *window;
Mutex resource_mutex;
int main(int argc, char** argv)
{
	// initialize openGL
    init_glfw(&window); 
	// setup glew for most recent openGL functions
    glewExperimental = GL_TRUE;
    glewInit(); // gets cool functions like glGenVertexArrays and glBindBuffer
	// Declare some openGL variables
	//TODO give these variables better names
    GLuint vao, ebo, vbo, v_shader, f_shader, shader_program, tex;
    GDALImage *image = create_gdal_image(argv[1]);
	// Setup shaders
	//TODO give this function a better name
    setup_polygons(&vao, &ebo, &vbo, &v_shader, &f_shader, &shader_program);
	// Declare variables to store window size
    int width, height;
	// Initialize the mutex
	init_mutex(resource_mutex);
    // main loop
    while(!glfwWindowShouldClose(window))
    {
		// Update the window size
		glfwGetWindowSize(window, &width, &height);
		// Resample the texture
		//TODO remove this function, it just calls another function
        check_texture(image, window, &tex);
		// Set clear color to black
        glClearColor(0,0,0,1);
		// Wipe the screen buffer 
        glClear(GL_COLOR_BUFFER_BIT);
		// Update screen buffer
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// Swap buffers to show on screen
        glfwSwapBuffers(window); // swap buffers (like vsync)
		// GFLW window manager function
        glfwPollEvents();
    }
	// Free the window
    glfwDestroyWindow(window);
    // cleanup
	// Release band buffers, etc
    destroy_gdal_image(image);
	// Delete the textures
    glDeleteTextures(1, &tex);
	// Delete all the stuff loaded to the GPU
    glDeleteProgram(shader_program);
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);
    glDeleteBuffers(1, &vbo);
	// Kill glfw
    glfwTerminate();
	// Free the mutex
	destroy_mutex(resource_mutex);
    return 0;
}
