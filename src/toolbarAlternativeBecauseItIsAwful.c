#include<GLFW/glfw3.h>
#include<IL/il.h>
#include<IL/ilu.h>
#include<IL/ilut.h>
/*#include"glext.h"*/

GLFWwindow *window;
int screen_width = 0, screen_height = 0;

int main()
{
    ILboolean running = true;
    init_glfw(&window, screen_width, screen_height);
    ilInit();
    iluInit();
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE,GL_TRUE);
    glfwOpenWindow( 800, 600, 0, 0, 0, 0, 0, 0, GLFW_WINDOW );
    ilutRenderer(ILUT_OPENGL);
    glfwSetWindowTitle("Toolbar");
    glEnable(GL_TEXTURE_RECTANGLE_ARB);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ILuint texid;
    ILboolean success;
    GLuint image;

    ilGenImages(1, &texid);
    ilBindImage(texid);
    ilLoadImage("background.bmp");
    //ilutGLLoadImage("background.bmp");
    //ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    glGenTextures(1, & image);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, image);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB,0,32,800,600,0, GL_RGBA, GL_UNSIGNED_BYTE,ilGetData());

    while(running)
    {
        glClear( GL_COLOR_BUFFER_BIT );
        glBegin(GL_QUADS);
        glTexCoord2i(0,1);
        glVertex2i(0,600);
        glTexCoord2i(1,1);
        glVertex2i(800,600);
        glTexCoord2i(1,0);
        glVertex2i(800,0);
        glTexCoord2i(0,0);
        glVertex2i(0,0);
        glEnd();
        glfwSwapBuffers();
        running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam( GLFW_OPENED);
    }
    ilDeleteImages(1, &texid);
    glDeleteTextures(1, & image);
    glfwTerminate();
    return 0;
}
