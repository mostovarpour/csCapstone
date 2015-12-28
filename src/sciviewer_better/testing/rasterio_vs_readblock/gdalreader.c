#include "gdalreader.h"
#include <sys/timeb.h>
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
    //TODO don't actually fail, simply 
    //throw an error gracefully and continue running
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
    image->num_blocks.x = image->original_width/image->block_size.x;
    image->num_blocks.y = image->original_height/image->block_size.y;
    DEFAULT(image->num_blocks.x, 0, 1);
    DEFAULT(image->num_blocks.y, 0, 1);
    int i;
    for(i = 0; i < image->band_count; i++)
    {
        image->is_sampling[i] = false;
        image->bands[i] = NULL;
    }
    // Print information about the file
    // TODO add verbose flag that will print this info
    puts("--------------------------------------------------------------------");
    print_file_information(image);
    puts("--------------------------------------------------------------------");
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
    printf("X Blocks = %d\n", image->num_blocks.x);
    printf("Y Blocks = %d\n", image->num_blocks.y);
}

///<summary>
///Doesn't actually downsample, well, it does, but the magic happens in rasterio
/// we may have to really get into GDAL to get the downsampling working the way we want it
///</summary>
void downsample(GDALImage *image, int width, int height)
{
    if(GDALGetRasterDataType(image->current_band) == GDT_Int16)
    {
        puts("GDT_16");
        exit(EXIT_FAILURE);
    }
    else
    {
        int i;
        // prepare threads
        // can't have image->band_count inside [] to declare array
        thread_params **thread_parameters = malloc(sizeof(thread_params *) * image->band_count); 
        thread *threads = malloc(sizeof(thread) * image->band_count); // create the threads just so that the create_thread function is happy, then fire and forget
        // set up thread parameters
        for(i = 0; i < image->band_count; i++)
        {
            // allocate space for band info
            // if space hasn't already been allocated
            if(image->bands[i] == NULL)
                image->bands[i] = (GByte *) CPLMalloc(sizeof(GByte) * width * height);
            // get space for the thread params, can't just
            // get memory from the stack since as soon as
            // this function ends the stack memory will be freed
            // and the threads won't have access anymore
            thread_parameters[i] = (thread_params *)malloc(sizeof(thread_params));
            thread_parameters[i]->width = width;
            thread_parameters[i]->height = height;
            thread_parameters[i]->buffer = image->bands[i];
            thread_parameters[i]->band = GDALGetRasterBand(image->dataset, i+1);
            thread_parameters[i]->is_sampling = &image->is_sampling[i];
            thread_parameters[i]->image = image;
            image->is_sampling[i] = true;
        }
        // create a thread for reading the image
        for(i = 0; i < image->band_count; i++)
        {
            /*create_thread(threads[i], fill_band, (thread_arg)(thread_parameters[i]));*/
            fill_band(thread_parameters[i]);
            printf("RasterIO: Band %d completed\n", i);
            CPLFree(image->bands[i]);
        }
        free(thread_parameters); // free the dynamic array, the actual pointers in the array are still out there
        free(threads);
    }
}
unsigned long long compare_time(struct timeb *one, struct timeb *two)
{
    if(one->time == two->time)
        return two->millitm - one->millitm;
    else if(one->time > two->time)
        return 0;
    else
    {
        two->time -= 1;
        two->millitm += 1000;
        return (two->time - one->time)*1000 + two->millitm - one->millitm;
    }
}

thread_func fill_band(thread_arg params)
{
    struct timeb time;
    ftime(&time);
    thread_params *in = (thread_params*)params;
    lock_mutex(resource_mutex);
    int width, height;
    width = GDALGetRasterBandXSize(in->band);
    height = GDALGetRasterBandYSize(in->band);
    GDALResult result = GDALRasterIO(in->band, GF_Read, 
            0, 0, width, height,
            in->buffer, in->width, in->height, GDT_Byte, 0, 0);
    if(result == CE_Failure)
    {
        fprintf(stderr, "Failed to read band\n");
    }
    struct timeb end;
    ftime(&end);
    printf("Time: %lldms\n", compare_time(&time, &end));
    // free memory allocated before threads were created 
    *in->is_sampling = false;
    if(!is_sampling(in->image))
        in->image->ready_to_upload = true;
    release_mutex(resource_mutex);
    free(params);
    return 0;
}

void sample(GDALImage *image, int width, int height) 
{
    // break if still not finished from last sampling
    // call
    bool sampling = is_sampling(image);
    // if we're sampling, ready to upload should be false
    if (sampling || image->ready_to_upload)
    {
        return;
    }
    // otherwise if we're not sampling and ready to upload
    // break if the image hasn't yet been uploaded to the gpu.
    // don't want to overwrite the buffer yet
    downsample(image, width, height);

}

// initialize is sampling and any other
// variables that should be defaulted and return the pointer
GDALImage *create_gdal_image(char *filepath)
{
    GDALImage *image =(GDALImage *) malloc(sizeof(GDALImage));
    image->filepath = filepath;
    image->ready_to_upload = false;
    image->should_sample = true;
    fill_image_data(image);
    return image;
}

void destroy_gdal_image(GDALImage *image)
{
    int i;
    // if the image is still sampling wait for it to finish
    // otherwise the program will segfault
    while(is_sampling(image));
    // free allocated band space
    for(i = 0; i < image->band_count; i++)
    {
        CPLFree(image->bands[i]);
    }
    // free allocated struct
    free(image);
}

bool is_sampling(GDALImage *image)
{
    int i;
    for(i = 0; i < image->band_count; i++)
    {
        if(image->is_sampling[i])
            return true;
    }
    return false;
}

void blockread(GDALImage *image)
{
    GDALRasterBandH band;
    // allocate size for data
    GByte *tmp;
    GDALResult result;
    for(int i = 0; i < image->band_count; i++)
    {
        band = GDALGetRasterBand(image->dataset, i+1);
        image->bands[i] = CPLMalloc(sizeof(GByte) * image->num_blocks.x * image->num_blocks.y * image->block_size.x * image->block_size.y);
        tmp = image->bands[i];
        for(int x = 0; x < image->num_blocks.x; x++)
        {
            for(int y = 0; y < image->num_blocks.y; y++)
            {
                result = GDALReadBlock(band, x, y, tmp);
                if(result == CE_Failure)
                    printf("Failed to read block (%d,%d)\n", x, y);
                tmp += image->block_size.x * image->block_size.y;
            }
        }
        printf("BlockRead: Band %d completed\n", i);
        CPLFree(image->bands[i]);
    }
}
