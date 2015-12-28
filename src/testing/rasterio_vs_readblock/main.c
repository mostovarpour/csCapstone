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
    sample(image, image->original_width, image->original_height);
    blockread(image);
    destroy_gdal_image(image);
}
