#include <stdlib.h>
#include <stdio.h>
#include "gdal.h"
#include "cpl_conv.h"

static int r = 0;

void initLevelOfDetail(int tiles, LevelOfDetail* lod);
int getNumLevels();
void initLODSVImage(LevelOfDetail *lod, int numTiles);

//void sample(int numTiles, LevelOfDetail* lod);


LevelOfDetail** initLODArray(int numLevels, int numTiles){
	LevelOfDetail** mLod;
	mLod = (LevelOfDetail**) malloc(sizeof(LevelOfDetail*)*numLevels);
	//allocate space for each level of detail
	int i;
	for(i =0 ; i< numLevels; i++){
		mLod[i] = (LevelOfDetail*) malloc(sizeof(LevelOfDetail));
		mLod[i]->scale = (1 << i);
	}
	
	//initialize levels of detail (allocates space for required output size)
	for(i =0 ; i< numLevels; i++){
		initLevelOfDetail(numTiles, mLod[i]);
		initLODSVImage(mLod[i],numTiles);
	}
	return mLod;
}

void initLevelOfDetail(int numTiles, LevelOfDetail* lod){
	int i;
	//allocate space for array of SVImages (one space for each tile in the image)
	lod->data = (SVImage**)malloc(sizeof(SVImage*) * numTiles);
}

void initLODSVImage(LevelOfDetail *lod, int numTiles){
	int i;
	for (i = 0; i<numTiles;i++){
		lod->data[i] = malloc(sizeof(SVImage));
	}
}

int getNumLevels(){
	int numLevels = 1;
	
	GDALDatasetH  hDataset;
	GDALAllRegister();
	
	//TODO Pass filepath as parameter
	
	hDataset = GDALOpen( filepath, GA_ReadOnly );
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
	
	int nativeRes = origWidth*origHeight;
	
	int displayRes;
	
	if (!glfwInit()){
		return -1;
	}
	
	const GLFWvidmode* res;

	res = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int resHeight = res->height;
	int resWidth = res->width;

	displayRes = (resWidth * resHeight);
	printf("Screen resolution is %i (%i x %i)\n",displayRes,resWidth,resHeight);
	while (1){
		nativeRes = (nativeRes * .25);
		if (nativeRes < displayRes){
			numLevels++;
			break;
		}else{
			numLevels++;
		}
	} 
	printf("There are %i levels\n",numLevels);
	GDALClose(hDataset);
	return numLevels;
}
