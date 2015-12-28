#include "headers/sampler.h"

/*
Creates an SVImage struct with CPLMalloced data section.  
*/
int scale = 16;
SVImage init(int xLow, int xHigh, int yLow, int yHigh, const char *filepath){

    GDALDatasetH  hDataset;
    // Register drivers
    GDALAllRegister();
    
    // open the given file as Read Only
    hDataset = GDALOpen(filepath, GA_ReadOnly );
    // if opening failed
    if( hDataset == NULL )
    {
        fprintf(stderr, "Unable to open file.");
        exit(EXIT_FAILURE);
    }

    /*      Declare some variables to be used later     */
    int *xBlockSize = malloc(sizeof(int));
    int *yBlockSize = malloc(sizeof(int));
    int xOutputSize;
    int yOutputSize;
    int numXBlocks;
    int numYBlocks;
    int origWidth;
    int origHeight;
    int origBandCount;
    
    /*Get some information on the image*/
    origWidth = GDALGetRasterXSize( hDataset );                 // Get raster pixel width
    origHeight = GDALGetRasterYSize( hDataset );                // Get raster pixel height
    origBandCount = GDALGetRasterCount( hDataset );             // Get number of raster bands in the image
    GDALRasterBandH hBand = GDALGetRasterBand( hDataset, 1 );   // Get raster band with id 1
    GDALGetBlockSize( hBand, xBlockSize, yBlockSize);           // Fetch the block size for this band

    /*TODO make sure scale is set somewhere*/
    
    /*Store some information on what the output should have*/
    xOutputSize = ((*xBlockSize) / scale); // get the new x block size
    yOutputSize = ((*yBlockSize) / scale); // get the new y block size
    numXBlocks = origWidth/(*xBlockSize);  // Get x block count
    numYBlocks = origHeight/(*yBlockSize); // Get y block count

    int bandCount;
    if (origBandCount >= 3) // Just a guess, limit bands to RGB?
    {
        bandCount = 3;
    }
    else                    // Otherwise image is probably grayscale, one band?
    {
        bandCount = 1;
    }

    /*TODO edit this or remove it since we aren't using focusing on openGL*/
    /*
     *int format;
     *if (bandCount == 3)
     *{
     *    format = GL_RGBA;
     *}
     *else
     *{
     *    format = GL_LUMINANCE_ALPHA;
     *}
     */
    
    int usedBlocks;
    int usedXBlocks;
    int usedYBlocks;
    /*
     *This is odd, xHigh and yHigh are
     *passed as parameters but changed here
     *TODO see if we can remove parameters or just use this
     */

    if ((xHigh < 0) || (xHigh < xLow))
    {
        xHigh = numXBlocks;
    }
    if ((yHigh < 0)|| (yHigh < yLow))
    {
        yHigh = numYBlocks;
    }
    
    usedXBlocks = (xHigh - xLow); // This is probably the part of the image that should be displayed on screen
    usedYBlocks = (yHigh - yLow); // Y part on screen?
    usedBlocks = (usedXBlocks * usedYBlocks); // Total blocks on screen?
    
    SVImage svip = 
        {
            .Width = xOutputSize*usedXBlocks,
            .Height = yOutputSize*usedYBlocks,
            .BytesPerPixel = 1, 
            .Data = (CPLMalloc((sizeof(char) * xOutputSize * yOutputSize * usedBlocks * (bandCount+1))))
            // Data = xOutputSize * yOutputSize = pixels/block * usedBlocks = totalpixels 
        };

    free(xBlockSize);
    free(yBlockSize);
    return svip;
}


/*
Uses stochastic sampling to fill the data section of an SVImage struct.
*/
void sample(params *in)		//TODO handle 32 bit representations
{
    /*Set variables from the params struct*/
    SVImage* out = in->target;
    int xLow = in->xLow;
    int xHigh = in->xHigh;
    int yLow = in->yLow;
    int yHigh = in->yHigh;
    const char *file = in->file;

    GDALDatasetH  hDataset;
    GDALRasterBandH hBand;
    /*Register drivers*/
    GDALAllRegister();
	
    //TODO Dynamically calculate sample count?
    int avgSampleCount = 25;
 
    //Open GDAL File
    //------------------------------------------------
    hDataset = GDALOpen(file, GA_ReadOnly );
    if( hDataset == NULL )
    {
    	fprintf(stderr, "Unable to open file.\n"); 
        exit(EXIT_FAILURE);
    }
    
    //GDAL FILE INFO
    //---------------------------------------------
    GDALDriverH   hDriver;
    hDriver = GDALGetDatasetDriver( hDataset ); // Get driver for this file

    double adfGeoTransform[6];

    //Print GDAL Driver 
    printf( "Driver: %s/%s\n",
            GDALGetDriverShortName( hDriver ),
            GDALGetDriverLongName( hDriver ) );

    //Get original raster size
    int origWidth = GDALGetRasterXSize( hDataset ); 
    int origHeight = GDALGetRasterYSize( hDataset ); 
    printf("width = %i\n", origWidth);
    printf("height = %i\n", origHeight);  

    //Get Raster band count	
    int origBandCount = GDALGetRasterCount( hDataset );
    printf("origBandCount = %i\n", origBandCount);
    int bandCount;
    if (origBandCount >= 3)
    {
        bandCount = 3;
    }
    else
    {
        bandCount = 1;
    }

    //Target output Width and Height
    float stride = (scale * scale);				
    stride /= (avgSampleCount);

    //the greatest number of pixels that can be skipped in a single iteration
    int maxStride = ((int)stride) + 1;				

    //Load band 1
    hBand = GDALGetRasterBand( hDataset, 1 );

    if( GDALGetGeoTransform( hDataset, adfGeoTransform ) == CE_None )
    {
        printf( "Pixel Size = (%.6f,%.6f)\n",
                    adfGeoTransform[1], adfGeoTransform[5] );
    }
    else
    {
        fprintf(stderr, "Failed to get pixel size\n");
    }

    int* xBlockSize = malloc(sizeof(int));
    int* yBlockSize = malloc(sizeof(int));
    //get block size
    GDALGetBlockSize( hBand, xBlockSize, yBlockSize);
    printf("xBlockSize = %i\n", *xBlockSize);
    printf("yBlockSize = %i\n", *yBlockSize);  
    int xOutputSize = ((*xBlockSize) / scale);
    int yOutputSize = ((*yBlockSize) / scale);
    printf("X Output Size%i\n", xOutputSize);
    int numXBlocks = origWidth/(*xBlockSize);
    int numYBlocks = origHeight/(*yBlockSize);
    printf("numXBlocks = %i\n", numXBlocks);
    printf("numYBlocks = %i\n", numYBlocks);
    
    if ((xHigh < 0) || (xHigh < xLow))
    {
        xHigh = numXBlocks;
    }
    if ((yHigh < 0)|| (yHigh < yLow))
    {
        yHigh = numYBlocks;
    }
    
    int usedXBlocks = (xHigh - xLow);
    int usedYBlocks = (yHigh - yLow);
    int usedBlocks = (usedXBlocks * usedYBlocks);
    
    unsigned char* output = CPLMalloc((sizeof(char) * xOutputSize* yOutputSize *usedBlocks  * (bandCount+1)));  // Allocate space for the output
    float* vals = calloc( xOutputSize* yOutputSize *usedBlocks * (bandCount+1), sizeof(float));			//stores pixel values
    unsigned long* valsPerIndex = calloc( xOutputSize* yOutputSize * usedBlocks * (bandCount+1), sizeof(unsigned long));		//stores number of pixel values per output index
    unsigned long rseed = 0;
    unsigned long rowIndex = 0;		//the index of the row to which we will output our value
    unsigned long colIndex = 0;		//the index of the column to which we will output our value
    unsigned long outIndex = 0;
    unsigned long sampledIndex = 0;	//One dimensional representation of column/row index
    
    //iterate through each pixel, jump to the last pixel we sampled.
    long long i = 0;
    int outputXOffset = 0;
    int outputYOffset = 0;
    int outputIndexOffset = 0;
    if (GDALGetRasterDataType(hBand) == GDT_Int16)
    {
        short* data = (short *) CPLMalloc(sizeof(unsigned short) * (*xBlockSize)*(*yBlockSize)); //TODO: GDAL Raster can be 16 bit pixel representation
        int band;
        for (band = 1; band <= bandCount; band++){
            hBand = GDALGetRasterBand( hDataset, band );
            int yBlock, xBlock;
            for(yBlock = yLow; yBlock < yHigh; yBlock++){
                for(xBlock = xLow; xBlock < xHigh; xBlock++){
                    if(GDALReadBlock(hBand,xBlock,yBlock, data) != CE_None)
                    {
                        fprintf(stderr, "Read block failed\n");
                        exit(EXIT_FAILURE);
                    }
                    for(i = 0 ; i < ((*xBlockSize)*(*yBlockSize)-1) ; i += rseed){
                        rseed = (214013 * rseed + 2531011); // Magic numbers.
                        rseed %= maxStride;	
                        sampledIndex = i;
                        i = (maxStride ==1) ? (i+1) : i;
                        rowIndex = (sampledIndex/(xOutputSize*scale* scale)) + ((yBlock - yLow)* yOutputSize);
                        colIndex = ((sampledIndex % (xOutputSize * scale))/scale) + ((xBlock - xLow) * xOutputSize);
                        outIndex = ((rowIndex * (xOutputSize * usedXBlocks ) + colIndex) * (bandCount+1) ) + (band -1);
                        vals[outIndex] += (*(data + sampledIndex));
                        vals[outIndex] += 118.450588 + ((*(data + sampledIndex))/128);
                        if (vals[outIndex]/valsPerIndex[outIndex] < 0) {
                            vals[outIndex] =0;
                        }else if (vals[outIndex]/valsPerIndex[outIndex] > 255){
                            vals[outIndex] = 255;
                        }
                    }
                }
            }
        }
    }
    else
    {
        unsigned char* data = (unsigned char *) CPLMalloc(sizeof(char) * (*xBlockSize)*(*yBlockSize));
        int band;
        for (band = 1; band <= bandCount; band++)
        {
            hBand = GDALGetRasterBand( hDataset, band );
            int yBlock, xBlock;
            outputYOffset = 0;
            for(yBlock = yLow; yBlock < yHigh; yBlock++){
                outputYOffset = ((yBlock - yLow) * yOutputSize * xOutputSize * usedXBlocks * (bandCount + 1));
                outputXOffset = 0;
                for(xBlock = xLow; xBlock < xHigh; xBlock++){
                    outputXOffset = ((xBlock - xLow) * xOutputSize * (bandCount + 1));
                    if(GDALReadBlock(hBand,xBlock,yBlock, data) != CE_None)
                    {
                        fprintf(stderr, "Read block failed\n");
                        exit(EXIT_FAILURE);
                    }
                    for(i = 0 ; i < ((*xBlockSize)*(*yBlockSize)) ; i += rseed){
                        rseed = (214013 * rseed + 2531011);
                        rseed %= maxStride;	
                        sampledIndex = i;
                        i = (maxStride ==1) ? (i+1) : i;
                        rowIndex = (sampledIndex/(xOutputSize*scale* scale)) + ((yBlock - yLow)* yOutputSize);
                        colIndex = ((sampledIndex % (xOutputSize * scale))/scale) + ((xBlock - xLow) * xOutputSize);
                        outIndex = ((rowIndex * (xOutputSize * usedXBlocks ) + colIndex) * (bandCount+1) ) + (band -1);
                        vals[outIndex] += (*(data + sampledIndex));
                        valsPerIndex[outIndex] +=1;
                    }
                    outputIndexOffset = (outputYOffset + outputXOffset);
                    int j;
                    for (j = 0; j<yOutputSize; j++){
                        i = outputIndexOffset;
                        int k = (i + (xOutputSize * (bandCount+1)));
                        for (i = 0; i<k;i++){
                            int x = (i+(j*(xOutputSize * (bandCount + 1) * (usedXBlocks))));
                            if(((x+1)%4==0&&bandCount==3)||((x+1)%2==0&&bandCount==1)){
                                output[x] = (unsigned char) 1; //Sets the alpha to opaque
                            }else{
                                output[x] = (unsigned char) (vals[x]/valsPerIndex[x]);	//calculate final output by averaging each color value
                            }
                        }
                    }
                    out->Data = output;
                }
            }
        }
    }

    printf("sampling complete\n");
    GDALClose(hDataset);
}
