#ifndef __GLHELPER_H_
#define __GLHELPER_H_
#include "os.h"
#include <GLFW/glfw3.h>
//#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include "args.h"
typedef GLFWwindow** ptGLFWwindow;
void init_glfw(ptGLFWwindow, int width, int height);
//GLuint png_texture_load(const char * file_name, int * width, int * height);
#endif
