#ifndef __PROBABILISTIC_H__
#define __PROBABILISTIC_H__
#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <GL/gl.h>
#include "gdal.h"
#include "cpl_conv.h" /* for CPLMalloc() */
#include "errcheck.h"
#include "threads.h"

#define DEFAULT_AVG_SAMPLE_COUNT 25

typedef struct coordinates {
    int x;
    int y;
} Point;

typedef struct GDALImageData
{
    const char *filepath; // path to image
    GDALDatasetH dataset; // gdal dataset
    GDALDriverH  driver; // driver for the image
    GDALRasterBandH current_band; // current raster band
    int original_width; // image width in pixels
    int original_height; // image height in pixels
    int band_count; // limited to 3 or less
    double geo_transform[6]; // transform data
    Point block_size; // x and y block size
    Point num_blocks; // num of x and y blocks
    GByte *bands[3]; // pointers to band pixel data
    bool is_sampling[3]; // store which bands are currently being sampled
    bool volatile should_sample; // flag determing if the image should be resampled
    bool volatile ready_to_upload; // boolean which signals that each band has finished loading and is ready to be sent to the gpu
} GDALImage;
typedef struct thread_params
{
    GByte *buffer;
    GDALImage *image;
    GDALRasterBandH band;
    int width;
    int height;
    bool *is_sampling;
} thread_params;

typedef int GDALResult;

int limit_band_count(int BandCount);
void sample(GDALImage *, int width, int height);
void print_file_information(GDALImage *);
// Requires that the filepath of GDALImage is already set
void fill_image_data(GDALImage *);
GDALImage *create_gdal_image(char *filepath);
void destroy_gdal_image(GDALImage *);
thread_func fill_band(void *);
bool is_sampling(GDALImage *);
#endif
