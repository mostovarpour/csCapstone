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

/*
 *GLuint png_texture_load(const char * file_name, int * width, int * height)
 *{
 *    png_byte header[8];
 *
 *    FILE *fp = fopen(file_name, "rb");
 *    if (fp == 0)
 *    {
 *        perror(file_name);
 *        return 0;
 *    }
 *
 *    // read the header
 *    fread(header, 1, 8, fp);
 *
 *    if (png_sig_cmp(header, 0, 8))
 *    {
 *        fprintf(stderr, "error: %s is not a PNG.\n", file_name);
 *        fclose(fp);
 *        return 0;
 *    }
 *
 *    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
 *    if (!png_ptr)
 *    {
 *        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
 *        fclose(fp);
 *        return 0;
 *    }
 *
 *    // create png info struct
 *    png_infop info_ptr = png_create_info_struct(png_ptr);
 *    if (!info_ptr)
 *    {
 *        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
 *        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
 *        fclose(fp);
 *        return 0;
 *    }
 *}
 */
