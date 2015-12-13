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
int main(int argc, char** argv)
{
    init_glfw(&window); // initialize openGL
    // In my case this isn't actually necessary which is kind of weird, but so it works
    // on all other machines it should be kept. I don't think windows is smart enough
    // to get those functions on its own even though linux may be
    glewExperimental = GL_TRUE;
    glewInit(); // gets cool functions like glGenVertexArrays and glBindBuffer
    GLuint vao, ebo, vbo, v_shader, f_shader, shader_program, tex;
    GDALImage *image = create_gdal_image(argv[1]);
    setup_polygons(&vao, &ebo, &vbo, &v_shader, &f_shader, &shader_program);
    // main loop
    while(!glfwWindowShouldClose(window))
    {
        check_texture(image, window, &tex);
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window); // swap buffers (like vsync)
        glfwPollEvents(); // glfw thing that manages window events
    }
    glfwDestroyWindow(window); // make sure the window is closed
    // cleanup
    destroy_gdal_image(image);
    glDeleteTextures(1, &tex);
    glDeleteProgram(shader_program);
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);
    glDeleteBuffers(1, &vbo);
    glfwTerminate(); // kill glfw
    return 0;
}
