#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
//GDAL includes:
//---------------------------------------------
#include "gdal.h"
#include "cpl_conv.h" /* for CPLMalloc() */
//---------------------------------------------
typedef struct {
	int Width, Height;   // Image dimensions
	int BytesPerPixel;   // Number of bytes per pixel
	unsigned char *Data; // Pointer to pixel data
} SVImage;

typedef struct{
	SVImage *target;
	int xLow;
	int xHigh;
	int yLow;
	int yHigh;
        const char *file;
} params;

SVImage init(int xLow, int xHigh, int yLow, int yHigh, const char *filepath);
void sample(params *in);
#endif
