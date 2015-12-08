#include "headers/probabilistic.h"
//TODO get scale as input
int scale = 16;

int limit_band_count(int count) {
    if(count >= 3) return 3;
    return 1;
}

void fill_image_data(GDALImage *image)
{
    GDALResult result;
    // Open GDAL File
    image->dataset = GDALOpen(image->filepath, GA_ReadOnly);
    FAILIF(image->dataset, NULL, "Unable to open file.");
    // Get file information
    image->driver         = GDALGetDatasetDriver(image->dataset);
    image->original_width  = GDALGetRasterXSize(image->dataset);
    image->original_height = GDALGetRasterYSize(image->dataset);
    image->band_count      = limit_band_count(GDALGetRasterCount(image->dataset));

    // Open first band to get block information
    image->current_band = GDALGetRasterBand(image->dataset, 1);
    result = GDALGetGeoTransform(image->dataset, image->geo_transform);
    FAILIF(result, CE_Failure, "Failed to get GeoTransform data");
    GDALGetBlockSize(image->current_band, &image->block_size.x, &image->block_size.y);
    image->output_size.x = image->block_size.x/image->scale;
    image->output_size.y = image->block_size.y/image->scale;
    image->num_blocks.x = image->original_height/image->block_size.x;
    image->num_blocks.y = image->original_height/image->block_size.y;
}

void print_file_information(GDALImage *image) 
{
    printf("Driver: %s/%s\n", GDALGetDriverShortName(image->driver), GDALGetDriverLongName(image->driver));
    printf("Width: %d\n", image->original_width);
    printf("Height: %d\n", image->original_height);
    printf("Band Count (limited to 3) %d\n", image->band_count);
    printf("Pixel Size = (%.6f,%.6f)\n", image->geo_transform[1], image->geo_transform[5]);
    printf("X Block size = %d\n", image->block_size.x);
    printf("Y Block size = %d\n", image->block_size.y);
    printf("X output size = %d\n", image->output_size.x);
    printf("Y output size = %d\n", image->output_size.y);
    printf("X Blocks = %d\n", image->num_blocks.x);
    printf("Y Blocks = %d\n", image->num_blocks.y);
}
void sample(sample_parameters *params) {
    printf("Opening %s", params->filepath);
    sample_prep(params->filepath, params->lod, DEFAULT_AVG_SAMPLE_COUNT);
}

// prepare for downsampling
void sample_prep(char* filepath, LevelOfDetail* lod, int avg_sample_count) 
{
    GDALImage image;
    /********************
     *FILE INITIALIZATION
     ********************/
    // Initialize drivers
    GDALAllRegister();

    // Get file information
    image.filepath = filepath;
    image.scale = scale;
    fill_image_data(&image);

#ifdef DEBUG
    // Print information about the file
    print_file_information(&image);
    printf("xLow: %i \n xHigh: %i \n yLow: %i\n yHigh: %i\n", *xLow, *xHigh, *yLow, *yHigh);
#endif   
    /*******************
     *END INITIALIZATION
     *******************/
    stochastic_sample(&image, avg_sample_count, lod);
}

void stochastic_sample(GDALImage *image, int avg_sample_count, LevelOfDetail *lod)
{
    float *vals;
    unsigned long seed, row_index, col_index, out_index, sample_index, *vals_per_index;
    long long i;
    int maxStride;
    double stride;
    int xBlockLow, xBlockHigh, yBlockLow, yBlockHigh;

    // get block range to display
    svGetVisibleTiles(&xBlockLow, &xBlockHigh, &yBlockLow, &yBlockHigh);

    // Calculate stride for reading
    stride = (scale * scale) / avg_sample_count;
    maxStride = ((int)stride) + 1;

    // Set bounds that we will should read
    if(xBlockHigh < 0 || xBlockHigh < xBlockLow)
        xBlockHigh = image->num_blocks.x;
    if(yBlockHigh < 0 || yBlockHigh < yBlockLow)
        yBlockHigh = image->num_blocks.y;
    
    // Possibly change these to band_count + 1
    vals = calloc(image->output_size.x * image->output_size.y * image->band_count, sizeof(float));
    vals_per_index = calloc(image->output_size.x * image->output_size.y * image->band_count, sizeof(long));
    seed = row_index = col_index = out_index = sample_index = i = 0;
    if(GDALGetRasterDataType(image->current_band) == GDT_Int16) 
    {
        puts("GDT_Int16");
    }
    else
    {
        // Allocate data for each block
        unsigned char *data = (unsigned char*) CPLMalloc(sizeof(char) * image->block_size.x * image->block_size.y * image->num_blocks.x * image->num_blocks.y);
        int band;
        for(band = 1; band <= image->band_count; band++)
        {
            // Get the current band
            image->current_band = GDALGetRasterBand(image->dataset, band);
            int x_block, y_block;
            // iterate through each block
            for(y_block = yBlockLow; y_block < yBlockHigh; y_block++)
            {
                for(x_block = xBlockLow; x_block < xBlockHigh; x_block++)
                {
                    printf("Doing something with block (%d, %d)\n", x_block, y_block);
                }
            }
        }
    }
}
