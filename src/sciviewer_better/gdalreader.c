#include "gdalreader.h"
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

void downsample(GDALImage *image, int width, int height)
{
    if(GDALGetRasterDataType(image->current_band) == GDT_Int16)
    {
        puts("GDT_16");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Allocate space for 1 value for each screen pixel
        GByte *data = (GByte *) CPLMalloc(sizeof(GByte) * width * height); 
        long x, y;
        x = GDALGetRasterBandXSize(image->current_band);
        y = GDALGetRasterBandYSize(image->current_band);
        GDALResult result = GDALRasterIO(image->current_band, GF_Read, 
                0, 0, x, y,
                data, width, height, GDT_Byte, 0, 0);
        if(result == CE_Failure)
        {
            fprintf(stderr, "Failed to read band");
            exit(EXIT_FAILURE);
        }
        puts("image read");
        image->band1 = data;
    }
}

// prepare for downsampling
void sample(const char* filepath, GDALImage *image, int width, int height) 
{
    // Get file information
    image->filepath = filepath;
    image->scale = 1;
    fill_image_data(image);
    //TODO create and call downsampling function
    downsample(image, width, height);

    // Print information about the file
    puts("--------------------------------------------------------------------");
    print_file_information(image);
    puts("--------------------------------------------------------------------");
}


