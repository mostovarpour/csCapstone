#include "headers/probabilistic.h"
int scale;  //data reduction scale.  We are creating a 1:Scale representation of the image.

/*
Uses stochastic sampling to fill the data section of an SVImage struct.
*/

void sample(LevelOfDetail* in) {		//TODO handle 32 bit representations
	int* xLow = malloc(sizeof(int));
	int* xHigh = malloc(sizeof(int));
	int* yLow = malloc(sizeof(int));
	int* yHigh = malloc(sizeof(int));
	
	svGetVisibleTiles(xLow, xHigh, yLow, yHigh);
	
    GDALDatasetH  hDataset;
    GDALAllRegister();
	GDALRasterBandH hBand;
	
	//TODO Dynamically calculate sample count?
	int avgSampleCount = 25;
 
  //Open GDAL File
  //------------------------------------------------
    hDataset = GDALOpen( filepath, GA_ReadOnly );
    if( hDataset == NULL )
    {
    	printf("Unable to open file."); 
		exit(0);
    }
	// File was opened successfully
	
	//GDAL FILE INFO
	//---------------------------------------------
	GDALDriverH   hDriver;
	double        adfGeoTransform[6];
	hDriver = GDALGetDatasetDriver( hDataset );

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
	if (origBandCount >= 3){
		bandCount = 3;
	}else{
		bandCount = 1;
	}
	//Target output Width and Height
	float stride = (scale * scale);				
	stride /= (avgSampleCount);
	//the greatest number of pixels that can be skipped in a single iteration
	int maxStride = ((int)stride) + 1;				
	//Load band 1
	hBand = GDALGetRasterBand( hDataset, 1 );
	if( GDALGetGeoTransform( hDataset, adfGeoTransform ) == CE_None ){
	printf( "Pixel Size = (%.6f,%.6f)\n",
					adfGeoTransform[1], adfGeoTransform[5] );
	}
	int* xBlockSize = malloc (sizeof(int));
	int* yBlockSize = malloc (sizeof(int));
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
	
	if ((*xHigh < 0) || (*xHigh < *xLow)){
		*xHigh = numXBlocks;
	}
	if ((*yHigh < 0)|| (*yHigh < *yLow)){
		*yHigh = numYBlocks;
	}
	
	int usedXBlocks = ((*xHigh) - (*xLow));
	int usedYBlocks = ((*yHigh) - (*yLow));
	int usedBlocks = (usedXBlocks * usedYBlocks);
	
	//unsigned char* output = CPLMalloc((sizeof(char) * xOutputSize* yOutputSize * (bandCount+1)));  // Allocate space for the output
	float* vals = calloc( xOutputSize* yOutputSize * (bandCount+1), sizeof(float));			//stores pixel values
	unsigned long* valsPerIndex = calloc( xOutputSize* yOutputSize * (bandCount+1), sizeof(unsigned long));		//stores number of pixel values per output index
	unsigned long rseed = 0;
	unsigned long rowIndex = 0;		//the index of the row to which we will output our value
	unsigned long colIndex = 0;		//the index of the column to which we will output our value
	unsigned long outIndex = 0;
	unsigned long sampledIndex = 0;	//One dimensional representation of column/row index
	//iterate through each pixel, jump to the last pixel we sampled.
	long long i = 0;
	long long e = 0;
	int j;
	printf("xLow: %i \n xHigh: %i \n yLow: %i\n yHigh: %i\n", *xLow, *xHigh, *yLow, *yHigh);
	
	/*
	for (i = 0; i<usedBlocks; i++){
		in->data[i]->Data = calloc(xOutputSize* yOutputSize * (bandCount+1), sizeof(unsigned char));
	}
	*/
	
	if (GDALGetRasterDataType(hBand) == GDT_Int16){	
		//TODO implement 16 bit
	}else{

		unsigned char* data = (char *) CPLMalloc(sizeof(char) * (*xBlockSize)*(*yBlockSize));
		int band = 1;
		for (band; band <= bandCount; band++){
		hBand = GDALGetRasterBand( hDataset, band );
			int yBlock = *yLow;
			int xBlock;
			for(yBlock; yBlock < *yHigh; yBlock++){
				xBlock = *xLow;
				//current yBlock (offset by lowest number of accessible y Blocks) * size of a block * number of used x Blocks * number of bands
				//printf("%i\n",yBlock);
				for(xBlock; xBlock < *xHigh; xBlock++){
					//printf("%i\n",xBlock);
					if(in->data[(yBlock * numXBlocks) + xBlock]->Sampled == 0){
						GDALReadBlock(hBand,xBlock,yBlock, data);
						for(i=0 ; i < ((*xBlockSize)*(*yBlockSize)) ; i += rseed){
							rseed = (214013 * rseed + 2531011);
							rseed %= maxStride;	
							sampledIndex = i;
							i = (maxStride == 1) ? (i+1) : i;
							rowIndex = (sampledIndex/(xOutputSize*scale* scale));
							colIndex = ((sampledIndex % (xOutputSize * scale))/scale);
							outIndex = (((rowIndex*xOutputSize) + colIndex));
							vals[outIndex] += (*(data + sampledIndex));
							valsPerIndex[outIndex] +=1;
						}
						
						for (i=0; i< ( xOutputSize* yOutputSize* (bandCount+1)); i+=4){
							//output[i] = (unsigned char) (vals[i]/valsPerIndex[i]);	//calculate final output by averaging each color value
							in->data[(yBlock * numXBlocks) + xBlock]->Data[i+ (band-1)] = (unsigned char) (vals[((i/4))]/valsPerIndex[((i/4))]);
							if(band ==1){
								in->data[(yBlock * numXBlocks) + xBlock]->Data[i+3] = (unsigned char) 255;
							}
						}

						//clear vals for the next block
						for (i=0; i< ( xOutputSize* yOutputSize* (bandCount+1)); i++){
							vals[i] = 0;
							valsPerIndex[i] = 0;
						}
						in->data[(yBlock * numXBlocks) + xBlock]->Width = xOutputSize;
						in->data[(yBlock * numXBlocks) + xBlock]->Height = yOutputSize;
						in->data[(yBlock * numXBlocks) + xBlock]->Format = GL_RGBA;
						in->data[(yBlock * numXBlocks) + xBlock]->BytesPerPixel = 1;
						if(band == bandCount){
							in->data[(yBlock * numXBlocks) + xBlock]->Sampled = 1;
						}
					}
				}
			}
		}
	}

	int currentlySampling = 0;

	printf("sampling complete\n");
	GDALClose(hDataset);
}
