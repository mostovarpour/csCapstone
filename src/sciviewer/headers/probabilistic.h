#ifndef __PROBABILISTIC_H__
#define __PROBABILISTIC_H__
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "lod2.h"
#include <GDAL/gdal.h>
#include <GDAL/cpl_conv.h>
#include "errcheck.h"

#define DEFAULT_AVG_SAMPLE_COUNT 25

typedef struct coordinates {
    int x;
    int y;
} Point;

typedef struct GDALImageData
{
    char *filepath;
    int scale;
    GDALDatasetH dataset;
    GDALDriverH  driver;
    GDALRasterBandH current_band;
    int original_width;
    int original_height;
    int band_count; // limited to 3 or less
    double geo_transform[6];
    Point block_size;
    Point num_blocks;
    Point output_size;
} GDALImage;
typedef struct sample_parms
{
    char *filepath;
    LevelOfDetail *lod;
    int x_low;
    int y_low;
    int x_high;
    int y_high;
} sample_parameters;

typedef int GDALResult;

int limit_band_count(int BandCount);
void sample(sample_parameters*);
void sample_prep(char* filepath, LevelOfDetail*, int);
void stochastic_sample(GDALImage *, int, LevelOfDetail*);
void print_file_information(GDALImage *);
// Requires that the filepath of GDALImage is already set
void fill_image_data(GDALImage *);
#endif
