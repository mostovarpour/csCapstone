#ifndef __GLHELPER_H_
#define __GLHELPER_H_
#include "os.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "args.h"
typedef GLFWwindow** ptGLFWwindow;
void init_glfw(ptGLFWwindow, int width, int height);
#endif
