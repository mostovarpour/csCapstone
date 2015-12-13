#include <GDAL/gdal.h>
#include <GDAL/gdal.h>
#include "os.h"
#include "threads.h"
#include "gdalreader.h"

typedef struct params 
{
    GByte *buffer;
    GDALImage *image;
    GDALRasterBandH band;
    int width;
    int height;
    bool *is_sampling;
} params;
Mutex resource_mutex;

thread_func my_thread_function(thread_arg arg)
{
	// convert from void pointer to params pointer
	params *parameters = (params *)arg; 
lock_mutex(resource_mutex);
	int width, height;
	width = GDALGetRasterBandXSize(parameters->band);
	height = GDALGetRasterBandYSize(parameters->band);
	GDALResult result = GDALRasterIO(parameters->band, GF_Read, 0, 0, width, height, parameters->buffer, 800, 600, GDT_Byte, 0, 0);
	if (result == CE_Failure)
		fprintf(stderr, "Failed to read band\n");
	*parameters->is_sampling = false;
	// free the malloced struct
release_mutex(resource_mutex);
	free(parameters);
	return 0;
}

int main()
{
	init_mutex(resource_mutex);
	GDALImage *image = create_gdal_image("C:\\Users\\Danny\\Desktop\\VBOXSHARED\\colorhillshade_mola128_mola64_merge_90Nto90S_Simp_clon0.tif");
	fill_image_data(image);
	int i;
	// Array of pointers to parameters
	params **parameters = malloc(sizeof(params*) * image->band_count);
	// Array of threads to make create_thread function happy
	thread *threads = malloc(sizeof(thread) * image->band_count);
	// set up parameters for threads
	for (i = 0; i < image->band_count; i++)
	{
		image->bands[i] = CPLMalloc(sizeof(GByte) * 800 * 600);
		// malloc space for a params struct
		parameters[i] = (params*)malloc(sizeof(params));
		// get raster band
		parameters[i]->band = GDALGetRasterBand(image->dataset, i + 1);
		// allocate buffer space
		parameters[i]->buffer = image->bands[i];
		parameters[i]->width = 800;
		parameters[i]->height = 600;
		parameters[i]->is_sampling = &image->is_sampling[i];
		parameters[i]->image = image;
		image->is_sampling[i] = true;
	}
	for (i = 0; i < image->band_count; i++)
	{
		create_thread(threads[i], my_thread_function, (thread_arg)parameters[i]);
	}
	// free malloced memory
	free(parameters);
	free(threads);
	//int i;
	//for (i = 0; i < image->band_count; i++)
	//{
	//	GDALRasterBandH band = GDALGetRasterBand(image->dataset, i + 1);
	//	image->bands[i] = CPLMalloc(sizeof(GByte) * 800 * 600);
	//	int width, height;
	//	width = GDALGetRasterBandXSize(band);
	//	height = GDALGetRasterBandYSize(band);
	//	GDALResult result = GDALRasterIO(band, GF_Read, 0, 0, width, height, image->bands[i], 800, 600, GDT_Byte, 0, 0);
	//	if (result == CE_Failure)
	//		fprintf(stderr, "Failed to read band\n");
	//	CPLFree(image->bands[i]);
	//	image->is_sampling[i] = false;
	//}
	while (1)
	{
		if (is_sampling(image))
			continue;
		break;
	}
	destroy_mutex(resource_mutex);
	destroy_gdal_image(image);
	return 0;
}
