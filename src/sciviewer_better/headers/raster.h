#ifndef __RASTERSHADER_H_
#define __RASTERSHADER_H_
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "gdalreader.h"
struct Raster 
{
    GLint position_slot;
    GLint color_slot;
    int size;
};

typedef struct Raster Raster;
GLint init_shaders(); // initializes the shaders and returns the program_id
void init_raster(Raster*, GLint program_id, GDALImage *image);
void render_raster(Raster*);
#endif
