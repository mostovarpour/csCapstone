#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
//#include <unistd.h>


//GDAL includes:
//---------------------------------------------
#include "gdal.h"
#include "cpl_conv.h" /* for CPLMalloc() */
//---------------------------------------------


typedef struct {
	int Width, Height; // Image dimensions
	int Format; // OpenGL pixel format
	int BytesPerPixel; // Number of bytes per pixel
	unsigned char *Data; // Pointer to pixel data
} SVImage;

typedef struct{
	SVImage* target;
	int xLow;
	int xHigh;
	int yLow;
	int yHigh;
}params;

int scale;  //data reduction scale.  We are creating a 1:Scale representation of the image.


/*
Creates an SVImage struct with CPLMalloced data section.  
*/
SVImage init(int xLow, int xHigh, int yLow, int yHigh){

    GDALDatasetH  hDataset;
	GDALAllRegister();
	
	//TODO Pass filepath as parameter
	
	hDataset = GDALOpen( "C:\\Users\\Terry\\Desktop\\CS476\\lunar.cub", GA_ReadOnly );
	//hDataset = GDALOpen( "D:\\CubFiles\\lunar.cub", GA_ReadOnly );
    if( hDataset == NULL )
    {
    	printf("Unable to open file."); 
		exit(0);
    }
    
	GDALRasterBandH hBand;
	hBand = GDALGetRasterBand( hDataset, 1 );
	int origWidth = GDALGetRasterXSize( hDataset ); 
	int origHeight = GDALGetRasterYSize( hDataset ); 
	int* xBlockSize = malloc (sizeof(int));
	int* yBlockSize = malloc (sizeof(int));
	GDALGetBlockSize( hBand, xBlockSize, yBlockSize);
	int xOutputSize = ((*xBlockSize) / scale);
	int yOutputSize = ((*yBlockSize) / scale);
	int numXBlocks = origWidth/(*xBlockSize);
	int numYBlocks = origHeight/(*yBlockSize);
	int origBandCount = GDALGetRasterCount( hDataset );
	int bandCount;
	if (origBandCount >= 3){
		bandCount = 3;
	}else{
		bandCount = 1;
	}
	int format;
	if (bandCount == 3){
		format = GL_RGBA;
	}else{
		format = GL_LUMINANCE_ALPHA;
	}
	
	int usedBlocks;
	int usedXBlocks;
	int usedYBlocks;
		if ((xHigh < 0) || (xHigh < xLow)){
		xHigh = numXBlocks;
	}
	if ((yHigh < 0)|| (yHigh < yLow)){
		yHigh = numYBlocks;
	}
	
	usedXBlocks = (xHigh - xLow);
	usedYBlocks = (yHigh - yLow);
	usedBlocks = (usedXBlocks * usedYBlocks);
	
	SVImage svip = {.Width = xOutputSize*usedXBlocks,
				   .Height = yOutputSize*usedYBlocks,
					.Format = format,
					.BytesPerPixel = 1, 
					.Data = (CPLMalloc((sizeof(char) * xOutputSize* yOutputSize * usedBlocks * (bandCount+1))))};
	//SVImage* svip = &svi;
	return svip;
}


/*
Uses stochastic sampling to fill the data section of an SVImage struct.
*/
void sample(params* in){		//TODO handle 32 bit representations
	SVImage* out = in->target;
	int xLow = in->xLow;
	int xHigh = in->xHigh;
	int yLow = in->yLow;
	int yHigh = in->yHigh;
    GDALDatasetH  hDataset;
    GDALAllRegister();
	GDALRasterBandH hBand;

	//TODO Pass pointer to path as a parameter
	
	//Hard-coded local path on Austin's PC
	char* str1 = "C:\\Users\\Terry\\Desktop\\CS476\\lunar.cub";
	//char* str2 = "C:\\Users\\Terry\\Desktop\\CS476\\lunar.dat";
	//Hard-Coded local path on alex's pc
	//char* str1 = "D:\\CubFiles\\lunar.cub";
    //str1 = "D:\\CubFiles\\Lunar_8gb.cub";	
	//str2 = "C:\\sciviewer\\lunar.dat";
	
	
	//TODO Dynamically calculate sample count?
	int avgSampleCount = 25;
 
  //Open GDAL File
  //------------------------------------------------
    hDataset = GDALOpen( str1, GA_ReadOnly );
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
	
	if ((xHigh < 0) || (xHigh < xLow)){
		xHigh = numXBlocks;
	}
	if ((yHigh < 0)|| (yHigh < yLow)){
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
	long long e = 0;
	int outputXOffset = 0;
	int outputYOffset = 0;
	int outputIndexOffset = 0;
	if (GDALGetRasterDataType(hBand) == GDT_Int16){	
		short* data = (short *) CPLMalloc(sizeof(unsigned short) * (*xBlockSize)*(*yBlockSize)); //TODO: GDAL Raster can be 16 bit pixel representation
		int band = 1;
		for (band; band <= bandCount; band++){
		hBand = GDALGetRasterBand( hDataset, band );
		//GDALReadBlock(hBand,1,1, data);
			int yBlock = yLow;
			int xBlock;
			for(yBlock; yBlock < yHigh; yBlock++){
				xBlock = xLow;
				for(xBlock; xBlock < xHigh; xBlock++){
					GDALReadBlock(hBand,xBlock,yBlock, data);
					i=0;
					for(i ; i < ((*xBlockSize)*(*yBlockSize)-1) ; i += rseed){
						rseed = (214013 * rseed + 2531011);
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
					out->Data = output;
				}
			}
		}
	}else{
		unsigned char* data = (char *) CPLMalloc(sizeof(char) * (*xBlockSize)*(*yBlockSize));
		int band = 1;
		for (band; band <= bandCount; band++){
		hBand = GDALGetRasterBand( hDataset, band );
			int yBlock = yLow;
			int xBlock;
			outputYOffset = 0;
			for(yBlock; yBlock < yHigh; yBlock++){
				xBlock = xLow;
				outputYOffset = ((yBlock - yLow) * yOutputSize * xOutputSize * usedXBlocks * (bandCount + 1));
				//outputYOffset = ((yBlock - yLow) * yOutputSize);
				outputXOffset = 0;
				for(xBlock; xBlock < xHigh; xBlock++){
					//outputXOffset = ((xBlock - xLow) * xOutputSize * yOutputSize);
					outputXOffset = ((xBlock - xLow) * xOutputSize * (bandCount + 1));
					GDALReadBlock(hBand,xBlock,yBlock, data);
					i=0;
					for(i ; i < ((*xBlockSize)*(*yBlockSize)) ; i += rseed){
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
					i = 0;
					//for (i; i< ( xOutputSize* yOutputSize * usedBlocks * (bandCount+1)); i++){
					//i = (((yBlock - yLow)*(((xOutputSize * yOutputSize)*(bandCount+1)) * usedXBlocks)) + ((xBlock - xLow) * (xOutputSize * yOutputSize * (bandCount+1))));
					//e = (i + (xOutputSize * yOutputSize * (bandCount + 1)));
					//outputIndexOffset = ((outputYOffset* usedXBlocks) + outputXOffset * (bandCount + 1));
					outputIndexOffset = (outputYOffset + outputXOffset);
					
					int j = 0;
					for (j; j<yOutputSize; j++){
						i = outputIndexOffset;
						int k = (i + (xOutputSize * (bandCount+1)));
						for (i; i<k;i++){
							int x = (i+(j*(xOutputSize * (bandCount + 1) * (usedXBlocks))));
							if(((x+1)%4==0&&bandCount==3)||((x+1)%2==0&&bandCount==1)){
								output[x] = (unsigned char) 1; //Sets the alpha to opaque
							}else{
								output[x] = (unsigned char) (vals[x]/valsPerIndex[x]);	//calculate final output by averaging each color value
						}
						}
					}
					/*
					for (i; i<(xOutputSize*yOutputSize*(bandCount+1)); i++){
						
						//printf("%i\n",x);
						if(((x+1)%4==0&&bandCount==3)||((x+1)%2==0&&bandCount==1)){
							output[x] = (unsigned char) 1; //Sets the alpha to opaque
						}else{
							output[x] = (unsigned char) (vals[x]/valsPerIndex[x]);	//calculate final output by averaging each color value
						}
					}
					*/
					out->Data = output;
				}
			}
		}
	}

	printf("sampling complete\n");
	GDALClose(hDataset);
}
