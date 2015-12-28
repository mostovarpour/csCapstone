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
    sample(image, 1366, 768);
    datasetread(image, 1366, 768);
    destroy_gdal_image(image);
}
