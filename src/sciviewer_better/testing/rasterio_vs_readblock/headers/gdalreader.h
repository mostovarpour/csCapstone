#ifndef __PROBABILISTIC_H__
#define __PROBABILISTIC_H__
#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <GL/gl.h>
#include <GDAL/gdal.h>
#include <GDAL/cpl_conv.h> /* for CPLMalloc() */
#include "errcheck.h"
#include "threads.h"

/***********
 * STRUCTS *
 ***********/
struct Point {
    int x;
    int y;
};

///<summary>
///Simple struct with x, y for coordinates or any other 2 point values
///</summary>
typedef struct Point Point;

struct GDALImageData
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
};
///<summary>
///A struct to hold all the data on a single image.
///</summary>
typedef struct GDALImageData GDALImage;

struct thread_params
{
    GByte *buffer;
    GDALImage *image;
    GDALRasterBandH band;
    int width;
    int height;
    bool *is_sampling;
};

///<summary>
///Struct to pass to thread functions
///</summary>
typedef struct thread_params thread_params;

/******************
 * END OF STRUCTS *
 ******************/





///<summary>
///Simple alias for int for usage where gdal returns a CE_None or CE_Failure, etc
///</summary>
typedef int GDALResult;
///<summary>
///Function that returns 3 if the number is > 3
///Otherwise just returns the number
///</summary>
int limit_band_count(int BandCount);
///<summary>
///Calls the downsampling function
///or just returns if the image is already being sampled
///</summary>
void sample(GDALImage *, int width, int height);
#define rasteriosample(i, j, k) sample(i,j,k)
void blockread(GDALImage *image);
void print_file_information(GDALImage *);
// Requires that the filepath of GDALImage is already set
void fill_image_data(GDALImage *);
GDALImage *create_gdal_image(char *filepath);
void destroy_gdal_image(GDALImage *);
thread_func fill_band(thread_arg);
bool is_sampling(GDALImage *);

// Mutex from main.c
extern Mutex resource_mutex;
#endif
