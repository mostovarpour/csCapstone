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
void sample(sample_parameters *params) {
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
    puts("--------------------------------------------------------------------");
    print_file_information(&image);
    puts("--------------------------------------------------------------------");
#endif   
    /*******************
     *END INITIALIZATION
     *******************/
    stochastic_sample(&image, avg_sample_count, lod);
}

void stochastic_sample(GDALImage *image, int avg_sample_count, LevelOfDetail *lod)
{
    float *vals;
    unsigned long seed, row_index, col_index, out_index, sample_index, *vals_per_index, current_index;
    long long i;
    int maxStride;
    double stride;
    int xBlockLow, xBlockHigh, yBlockLow, yBlockHigh;
    srand(time(NULL));
    // get block range to display
    svGetVisibleTiles(&xBlockLow, &xBlockHigh, &yBlockLow, &yBlockHigh);

    /*if(xBlockHigh < 0 || xBlockHigh < xBlockLow)*/
        xBlockHigh = image->num_blocks.x;
    /*if(yBlockHigh < 0 || yBlockHigh < yBlockLow)*/
        yBlockHigh = image->num_blocks.y;
#ifdef DEBUG
    printf("xLow: %i \nxHigh: %i\nyLow: %i\nyHigh: %i\n", xBlockLow, xBlockHigh, yBlockLow, yBlockHigh);
#endif

    // Calculate stride for reading
    stride = (scale * scale) / avg_sample_count;
    maxStride = ((int)stride) + 1;

    // Set bounds that we will should read
    
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
        GByte *data = CPLMalloc(image->block_size.x * image->block_size.y);
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
#ifdef DEBUG
                    printf("Reading Block (%d, %d)\n", x_block, y_block);
#endif
                    if(GDALReadBlock(image->current_band, x_block, y_block, data) == CE_Failure){
                        printf("Failed to read block (%d, %d)\n", x_block, y_block);
                        exit(1);
                    }
                    for(i = 0; i < image->block_size.x*image->block_size.y; i += seed) 
                    {
                        seed = rand() * rand();
                        seed %= maxStride;
                        row_index = i/(image->output_size.x * scale * scale);
                        col_index = (i % (image->output_size.x * scale))/scale;
                        out_index = row_index*image->output_size.x + col_index;
                        vals[out_index] += *(data + i);
                        vals_per_index += 1;
                    }
                    current_index = y_block * image->num_blocks.x + x_block;
                    for(i = 0; i < image->output_size.x * image->output_size.y * image->band_count + 1; i += 4)
                    {
                        // TODO possibly make this if/else
                        lod->data[current_index]->Data[i + band - 1] = vals[i/4]/vals_per_index[i/4];
                        if(band == 1)
                        {
                            lod->data[current_index]->Data[i+3] = 255;
                        }
                    }
                    lod->data[current_index]->Width = image->output_size.x;
                    lod->data[current_index]->Height = image->output_size.y;
                    lod->data[current_index]->Format = GL_RGBA;
                    lod->data[current_index]->BytesPerPixel = 1;
                    if(band == image->band_count)
                        lod->data[current_index]->Sampled = 1;
                }
            }
        }
    }
    GDALClose(image->dataset);
    puts("Done Sampling");
    exit(0);
}
