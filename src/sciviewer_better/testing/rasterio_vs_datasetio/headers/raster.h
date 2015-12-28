#ifndef __RASTERSHADER_H_
#define __RASTERSHADER_H_
#include "os.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "gdalreader.h"
void setup_polygons(GLuint *vertex_attribute_obj, GLuint *element_buffer, GLuint *vertex_buffer, GLuint *v_shader, GLuint *f_shader, GLuint *shader_program);
void setup_texture(GLFWwindow *window, GDALImage *image, GLuint *tex, GLuint shader);
#endif
