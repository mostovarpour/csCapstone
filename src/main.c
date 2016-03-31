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
#include "toolbar.h"
GLFWwindow *window;
Mutex resource_mutex;
int *button_width = NULL, *button_height = NULL;
int screen_width = 0, screen_height = 0;
char *file_path = NULL;

#define DEFAULT_WIDTH  128
#define DEFAULT_HEIGHT 128

int width  = DEFAULT_WIDTH;
int height = DEFAULT_HEIGHT;

/* Handler for window-repaint event. Called back when the window first appears and
   whenever the window needs to be re-painted. */
void display() 
{
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix

    /* Draw a quad */
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0,   0);
    glTexCoord2i(0, 1); glVertex2i(0,   height);
    glTexCoord2i(1, 1); glVertex2i(width, height);
    glTexCoord2i(1, 0); glVertex2i(width, 0);
    glEnd();

    glutSwapBuffers();
} 

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei newwidth, GLsizei newheight) 
{  
    // Set the viewport to cover the new window
    glViewport(0, 0, width=newwidth, height=newheight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, height, 0.0, 0.0, 100.0);
    glMatrixMode(GL_MODELVIEW);

    glutPostRedisplay();
}


/* Initialize OpenGL Graphics */
/*
 *void initGL(int w, int h) 
 *{
 *     glViewport(0, 0, w, h); // use a screen size of WIDTH x HEIGHT
 *     glEnable(GL_TEXTURE_2D);     // Enable 2D texturing
 * 
 *    glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
 *     glLoadIdentity();
 *     glOrtho(0.0, w, h, 0.0, 0.0, 100.0);
 * 
 *     glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling
 * 
 *     glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
 *     glClearDepth(0.0f);
 *     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window
 *}
 */

/* Load an image using DevIL and return the devIL handle (-1 if failure) */
int LoadImage(char *filename)
{
    ILboolean success; 
    ILuint image; 

    ilGenImages(1, &image); /* Generation of one image name */
    ilBindImage(image); /* Binding of image name */
    success = ilLoadImage(filename); /* Loading of the image filename by DevIL */

    if (success) /* If no error occured: */
    {
        /* Convert every colour component into unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
        success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); 

        if (!success)
        {
            return -1;
        }
    }
    else return -1;

    return image;
}


int main(int argc, char** argv)
{
    GLuint texid;
    int    picImage;

    if ( argc < 1)
    {
        /* no image file to  display */
        return -1;
    }

    /* GLUT init */
    glutInit(&argc, argv);            // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
    glutInitWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);   // Set the window's initial width & height
    //TODO Using TODO to mark this as something that I just changed
    //glutCreateWindow(argv[0]);      // Create window with the name of the executable
    glutDisplayFunc(display);       // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);       // Register callback handler for window re-size event

    /* OpenGL 2D generic init */
    /*initGL(DEFAULT_WIDTH, DEFAULT_HEIGHT);*/

    /* Initialization of DevIL */
    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
    {
        printf("wrong DevIL version \n");
        return -1;
    }
    ilInit(); 


    /* load the file picture with DevIL */
    picImage = LoadImage(argv[2]);
    /*picImage = LoadImage("../utilityIcons/panLeft.jpg");*/
    printf("%s\n", picImage);
    if ( picImage == -1 )
    {
        printf("Can't load picture file %s by DevIL \n", argv[2]);
        return -1;
    }

    /* OpenGL texture binding of the image loaded by DevIL  */
    glGenTextures(1, &texid); /* Texture name generation */
    glBindTexture(GL_TEXTURE_2D, texid); /* Binding of texture name */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); /* We will use linear interpolation for magnification filter */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); /* We will use linear interpolation for minifying filter */
    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
            0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData()); /* Texture specification */

    /*==============================================
     * This is where the normal working stuff starts
     *==============================================*/

    /* Main loop */
    glutMainLoop();

    /* Delete used resources and quit */
    //ilDeleteImages(1, &picImage); /* Because we have already copied image data into texture data we can release memory used by image. */
    glDeleteTextures(1, &texid);


    // check parameters and update variables accordingly
    args_check(argc, argv);
    // initialize openGL
    init_glfw(&window, screen_width, screen_height);

    // setup glew for most recent openGL functions
    glewExperimental = GL_TRUE;
    glewInit(); // gets cool functions like glGenVertexArrays and glBindBuffer
    glfwGetWindowSize(window, &screen_width, &screen_height);

    // Declare some openGL variables
    GDALImage *image = create_gdal_image(file_path);
    GLuint vertex_attribute_obj, element_buffer, vertex_buffer, v_shader, f_shader, shader_program
        , *texture_buffer = (GLuint*)malloc(sizeof(GLuint) * image->band_count);

    // Setup shaders
    //TODO give this function a better name
    setup_polygons(&vertex_attribute_obj, &element_buffer, &vertex_buffer, &v_shader, &f_shader, &shader_program, screen_height);

    // Initialize the mutex
    init_mutex(resource_mutex);

    // main loop
    /*LoadImage("../utilityIcons/panDown.jpg");*/
    /*display();*/
    while(!glfwWindowShouldClose(window))
    {
        // Update the window size
        glfwGetWindowSize(window, &screen_width, &screen_height);
        // Resample the texture
        setup_texture(window, image, texture_buffer, shader_program);
        //Load the images for the toolbar
        /*char* panUp = (int**)LoadImage("../utilityIcons/panUp.jpg");*/
        printf("Debug1\n");
        /*LoadImage("../utilityIcons/panDown.jpg");*/
        /*printf("Debug2\n");*/
        /*LoadImage("../utilityIcons/panLeft.jpg");*/
        /*printf("Debug3\n");*/
        /*LoadImage("../utilityIcons/panRight.jpg");*/
        /*printf("Debug4\n");*/
        /*LoadImage("../utilityIcons/zoomOut.jpg");*/
        /*printf("Debug5\n");*/
        /*LoadImage("../utilityIcons/zoomIn.jpg");*/
        /*printf("Debug6\n");*/
        /*display();*/

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
