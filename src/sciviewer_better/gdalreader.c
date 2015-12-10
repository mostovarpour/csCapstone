#include "gdalreader.h"
//TODO get scale as input
int scale = 16;

int limit_band_count(int count) {
    if(count >= 3) return 3;
    return 1;
}

void fill_image_data(GDALImage *image)
{
    GDALAllRegister();
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
    image->num_blocks.x = image->original_width/image->block_size.x;
    image->num_blocks.y = image->original_height/image->block_size.y;

    DEFAULT(image->num_blocks.x, 0, 1);
    DEFAULT(image->num_blocks.y, 0, 1);
    DEFAULT(image->output_size.x, 0, 1);
    DEFAULT(image->output_size.y, 0, 1);
    /*fill_bands(image);*/
}

/*
 *void fill_bands(GDALImage *image)
 *{
 *    image->data = (float *) CPLMalloc(sizeof(float) * 800 * 600);
 *    [>if(GDALDatasetRasterIO(image->dataset, GF_Read, 0, 0, image->original_width, image->original_height, image->data, 800, 600, GDT_Float32, 3, NULL, 0, 0, 0) == CE_Failure)<]
 *    [>{<]
 *        [>puts("Error reading raster bands");<]
 *        [>exit(1);<]
 *    [>}<]
 *    if(GDALRasterIO(image->current_band, GF_Read, 0, 0, image->original_width, image->original_height, image->data, 800, 600, GDT_Float32, 0, 0)  == CE_Failure)
 *    {
 *        puts("Error reading raster bands");
 *        exit(1);
 *    }
 *}
 */

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

// prepare for downsampling
void sample(char* filepath, GDALImage *image) 
{
    // Get file information
    image->filepath = filepath;
    image->scale = scale;
    fill_image_data(image);
    
    //TODO create and call downsampling function

#ifdef DEBUG
    // Print information about the file
    puts("--------------------------------------------------------------------");
    print_file_information(&image);
    puts("--------------------------------------------------------------------");
#endif   
}

void downsample(GDALImage *image)
{
    if(GDALGetRasterDataType(image->current_band) == GDT_Int16)
    {
        puts("GDT_16");
        exit(EXIT_FAILURE);
    }
    else
    {
        GByte *data = (GByte *) CPLMalloc(sizeof(char) * image->block_size.x * image->block_size.y); // buffer for image data
    }
}
