#include "os.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "glhelper.h"
#include "gdalreader.h"
#include "raster.h"
#include "args.h"
GLFWwindow *window;
Mutex resource_mutex;
int screen_width = 0, screen_height = 0;
char *file_path = NULL;
int main(int argc, char** argv)
{
    // check parameters and update variables accordingly
    args_check(argc, argv);
    // initialize openGL
    init_glfw(&window, screen_width, screen_height); 
    // setup glew for most recent openGL functions
    glewExperimental = GL_TRUE;
    glewInit(); // gets cool functions like glGenVertexArrays and glBindBuffer
    glfwGetWindowSize(window, &screen_width, &screen_height);
    GDALImage *image = create_gdal_image(file_path, screen_width, screen_height);
    // Declare some openGL variables
    GLuint vertex_attribute_obj, element_buffer, vertex_buffer, v_shader, f_shader, shader_program
        , *texture_buffer = (GLuint*)malloc(sizeof(GLuint) * image->band_count);
    // Setup shaders
    //TODO give this function a better name
    setup_polygons(&vertex_attribute_obj, &element_buffer, &vertex_buffer, &v_shader, &f_shader, &shader_program);
    // Initialize the mutex
    init_mutex(resource_mutex);
    // main loop
    while(!glfwWindowShouldClose(window))
    {
        // Update the window size
        glfwGetWindowSize(window, &screen_width, &screen_height);
        // Resample the texture
        setup_texture(window, image, texture_buffer, shader_program);
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
    // Delete the textures
    glDeleteTextures(image->band_count, texture_buffer);
    // Release band buffers, etc
    destroy_gdal_image(image);
    free(texture_buffer);
    // Delete all the stuff loaded to the GPU
    glDeleteProgram(shader_program);
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);
    glDeleteBuffers(1, &vertex_buffer);
    // Kill glfw
    glfwTerminate();
    // Free the mutex
    destroy_mutex(resource_mutex);
    return 0;
}
