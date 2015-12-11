#ifndef __RASTERSHADER_H_
#define __RASTERSHADER_H_
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "gdalreader.h"
void setup_polygons(GLuint *vao, GLuint *ebo, GLuint *vbo, GLuint *v_shader, GLuint *f_shader, GLuint *shader_program);
void setup_texture(GLuint shader_program, GLFWwindow *window, 
        const char *filepath, GDALImage *image, GLuint *tex);
#endif
