#include <gdal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
void printUsage();
void printFileInformation(GDALDatasetH);
void profileStochastic(int, char *);
void profileNearestNeighbor(int, char *);
void profileDatasetIO(int tests, char *filepath);
void profileDatasetIONN(int tests, char *filepath);

const int WIDTH = 800;
const int HEIGHT = 600;
int skipMultiplier;
int main(int argc, char** argv)
{
    if(argc < 4)
    {
        printUsage();
        return 1;
    }
    char *filepath = argv[1];
    int numTests = atoi(argv[2]);
    skipMultiplier = atoi(argv[3]);
    GDALAllRegister();
    GDALDatasetH dataset = GDALOpen(filepath, GA_ReadOnly);
    if(dataset == NULL)
    {
        puts("Unable to open file");
        exit(EXIT_FAILURE);
    }
    printFileInformation(dataset);
    GDALClose(dataset);
    
    // Test the stochastic algorithm
    profileStochastic(numTests, filepath);

    profileNearestNeighbor(numTests, filepath);
    //profileDatasetIO(numTests, filepath);
    //profileDatasetIONN(numTests, filepath);
}

void profileStochastic(int tests, char *filepath)
{
    int i;
    printf("Running stochastic alg %d times\n", tests);
    for(i = 0; i < tests; i++)
    {   
        //printf("Run %d: ", i);
        GDALDatasetH ds = GDALOpen(filepath, GA_ReadOnly);
        int bandCount = GDALGetRasterCount(ds);
        if(bandCount > 3)
            bandCount = 3;

        // declare time variables
        struct timeval startTime, endTime, difference;
        gettimeofday(&startTime, NULL);

        // Run the algorithm
        int bandIndex;
        for(bandIndex = 1; bandIndex <= bandCount; bandIndex++)
        {
            GByte *buffer = (GByte *) malloc(sizeof(GByte) * WIDTH * HEIGHT);
            GDALRasterIOExtraArg extraArg;
            INIT_RASTERIO_EXTRA_ARG(extraArg);
            extraArg.eResampleAlg = GRIORA_Stochastic;
            extraArg.irandPixSampSize = skipMultiplier;
            GDALRasterBandH band = GDALGetRasterBand(ds, bandIndex);
            CPLErr result = GDALRasterIOEx(band, GF_Read, 0, 0, GDALGetRasterBandXSize(band), GDALGetRasterBandYSize(band),
                    buffer, WIDTH, HEIGHT, GDT_Byte, 0, 0, &extraArg);
            free(buffer);
            if(result == CE_Failure)
            {
                puts("Failed to read band");
                GDALClose(ds);
                exit(EXIT_FAILURE);
            }
        }

        gettimeofday(&endTime, NULL);
        timersub(&endTime, &startTime, &difference);
        //printf("Image sampled in %ld.%06ld\n", difference.tv_sec, difference.tv_usec);
        printf("%ld.%06ld,", difference.tv_sec, difference.tv_usec);
        GDALClose(ds);
    }
}

void profileNearestNeighbor(int tests, char *filepath)
{
    int i;
    printf("Running nearest neighbor alg %d times\n", tests);
    for(i = 0; i < tests; i++)
    {   
        //printf("Run %d: ", i);
        GDALDatasetH ds = GDALOpen(filepath, GA_ReadOnly);
        int bandCount = GDALGetRasterCount(ds);
        if(bandCount > 3)
            bandCount = 3;

        // declare time variables
        struct timeval startTime, endTime, difference;
        gettimeofday(&startTime, NULL);

        // Run the algorithm
        int bandIndex;
        for(bandIndex = 1; bandIndex <= bandCount; bandIndex++)
        {
            GByte *buffer = (GByte *) malloc(sizeof(GByte) * WIDTH * HEIGHT);
            GDALRasterIOExtraArg extraArg;
            INIT_RASTERIO_EXTRA_ARG(extraArg);
            GDALRasterBandH band = GDALGetRasterBand(ds, bandIndex);
            CPLErr result = GDALRasterIOEx(band, GF_Read, 0, 0, GDALGetRasterBandXSize(band), GDALGetRasterBandYSize(band),
                    buffer, WIDTH, HEIGHT, GDT_Byte, 0, 0, &extraArg);
            free(buffer);
            if(result == CE_Failure)
            {
                puts("Failed to read band");
                GDALClose(ds);
                exit(EXIT_FAILURE);
            }
        }

        gettimeofday(&endTime, NULL);
        timersub(&endTime, &startTime, &difference);
        //printf("Image sampled in %ld.%06ld\n", difference.tv_sec, difference.tv_usec);
        printf("%ld.%06ld,", difference.tv_sec, difference.tv_usec);
        GDALClose(ds);
    }   
}

void profileDatasetIO(int tests, char *filepath)
{
    int i;
    printf("Running datasetio with stochastic alg %d times\n", tests);
    for(i = 0; i < tests; i++)
    {   
        printf("Run %d: ", i);
        GDALDatasetH ds = GDALOpen(filepath, GA_ReadOnly);
        int bandCount = GDALGetRasterCount(ds);
        if(bandCount > 3)
            bandCount = 3;

        // declare time variables
        struct timeval startTime, endTime, difference;
        gettimeofday(&startTime, NULL);

        // Run the algorithm
            GByte *buffer = (GByte *) malloc(sizeof(GByte) * WIDTH * HEIGHT * bandCount);
            GDALRasterIOExtraArg extraArg;
            INIT_RASTERIO_EXTRA_ARG(extraArg);
            extraArg.irandPixSampSize = skipMultiplier;
            extraArg.eResampleAlg = GRIORA_Stochastic;
            CPLErr result = GDALDatasetRasterIOEx(ds, GF_Read, 0, 0, GDALGetRasterXSize(ds), GDALGetRasterYSize(ds),
                    buffer, WIDTH, HEIGHT, GDT_Byte, bandCount, 0, 0, 0, 0, &extraArg);
            free(buffer);
            if(result == CE_Failure)
            {
                puts("Failed to read band");
                GDALClose(ds);
                exit(EXIT_FAILURE);
            }

        gettimeofday(&endTime, NULL);
        timersub(&endTime, &startTime, &difference);
        printf("Image sampled in %ld.%06ld\n", difference.tv_sec, difference.tv_usec);
        GDALClose(ds);
    } 
}

void printFileInformation(GDALDatasetH dataset)
{
    int bandCount = GDALGetRasterCount(dataset);
    if(bandCount > 3)
        bandCount = 3;
    printf("Driver: %s/%s\n"
            "Width: %d\n"
            "Height: %d\n"
            "Band Count (Limited to 3): %d\n",
            GDALGetDriverShortName(dataset),
            GDALGetDriverLongName(dataset),
            GDALGetRasterXSize(dataset),
            GDALGetRasterYSize(dataset),
            bandCount);
    puts("");
}

void profileDatasetIONN(int tests, char *filepath)
{
    int i;
    printf("Running datasetio with nearest neighbor alg %d times\n", tests);
    for(i = 0; i < tests; i++)
    {   
        printf("Run %d: ", i);
        GDALDatasetH ds = GDALOpen(filepath, GA_ReadOnly);
        int bandCount = GDALGetRasterCount(ds);
        if(bandCount > 3)
            bandCount = 3;

        // declare time variables
        struct timeval startTime, endTime, difference;
        gettimeofday(&startTime, NULL);

        // Run the algorithm
            GByte *buffer = (GByte *) malloc(sizeof(GByte) * WIDTH * HEIGHT * bandCount);
            GDALRasterIOExtraArg extraArg;
            INIT_RASTERIO_EXTRA_ARG(extraArg);
            CPLErr result = GDALDatasetRasterIOEx(ds, GF_Read, 0, 0, GDALGetRasterXSize(ds), GDALGetRasterYSize(ds),
                    buffer, WIDTH, HEIGHT, GDT_Byte, bandCount, 0, 0, 0, 0, &extraArg);
            free(buffer);
            if(result == CE_Failure)
            {
                puts("Failed to read band");
                GDALClose(ds);
                exit(EXIT_FAILURE);
            }

        gettimeofday(&endTime, NULL);
        timersub(&endTime, &startTime, &difference);
        printf("Image sampled in %ld.%06ld\n", difference.tv_sec, difference.tv_usec);
        GDALClose(ds);
    } 
}
void printUsage()
{
    puts("USAGE: speedtest <image file> <# of runs>");
}
