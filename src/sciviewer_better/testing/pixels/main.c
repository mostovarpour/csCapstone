#include <GDAL/gdal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "os.h"
#include "threads.h"
#include "gdalreader.h"
Mutex resource_mutex;
int main(int argc, char **argv)
{
    GDALImage *image = create_gdal_image(argv[1]);
    GByte *data = CPLMalloc(sizeof(GByte) * image->original_width * image->original_height);
    GByte *tmp = data;
    GDALResult r;
    for(int i = 0; i < image->original_width; i++)
    {
        for(int j = 0; j < image->original_height; j++)
        {
            printf("reading pixel (%d, %d)\n", i, j);
            r = GDALRasterIO(image->current_band, GF_Read,
                    i,j,1,1,tmp,1,1, GDT_Byte, 0, 0);
            if(r == CE_Failure)
                puts("failed");
            tmp++;
        }
    }
    destroy_gdal_image(image);
}
