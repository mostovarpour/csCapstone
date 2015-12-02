#define GLFW_INCLUDE_ES2 1
#define GLFW_DLL 1


char* filepath = "/home/leah/Pictures/mars.tif";
//char* filepath = "D:\\CubFiles\\lunar.cub";
int currentlySampling = 0;
int resampled = 0;

static float dZoomLevel = 1.0;
static float zoomLevel = 1.0;
static float verticalOffset = 0.0;
static float dVerticalOffset = 0.0;
static float horizontalOffset = 0.0;
static float dHorizontalOffset = 0.0;
//TODO Determine number of levels
static int numLevels = 6;
static int currentLOD = 5;


#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "SV.h"
#include "probabilistic7.c"
#include "ui_images.c"
#include <pthread.h>


/**********************************/

GLFWwindow* window;
rasterGroup* initialRasterGroup;
rasterGroup* resampleRasterGroup;
LevelOfDetail** lodArray;
/**********************************/



const rasterVertex RasterVertices[] = {
  {1, -1, 0},
  {1, 1, 0},
  {-1, 1, 0},
  {-1, -1, 0}
};



const GLubyte Indices[] = {
  0, 1, 2,
  2, 3, 0
};

static const GLfloat squareTexCoords[] = {
        1, 1,
        1, 0,
        0, 0,
        0, 1
};
/*
  static const GLfloat squareTexCoords[] = {
        0, 1,
		1, 1,
        1, 0,
        0, 0
 };
*/
/**********************************/


void tweenIn(){
	while (zoomLevel < dZoomLevel){
		zoomLevel += (.001) * zoomLevel/10;
		sleep(1);
	}	
}

void tweenOut(){
	while (zoomLevel > dZoomLevel){
		zoomLevel -= (.001) * zoomLevel/10;
		sleep(1);
	}	
}

void tweenPanUp(){
	while(verticalOffset > dVerticalOffset){
		verticalOffset -= (.001 / zoomLevel);
		sleep(3);
	}
}

void tweenPanDown(){
	while(verticalOffset < dVerticalOffset){
		verticalOffset += (.001 / zoomLevel);
		sleep(3);
	}
}

void tweenPanLeft(){
	while(horizontalOffset < dHorizontalOffset){
		horizontalOffset += (.001 / zoomLevel);
		sleep(3);
	}
}

void tweenPanRight(){
	while(horizontalOffset > dHorizontalOffset){
		horizontalOffset -= (.001 / zoomLevel);
		sleep(3);
	}
}

char* raster_vertex_shader_src =
  "attribute vec4 Position1;\n"
  "attribute vec2 SourceColor1;\n"
  "\n"
  "varying vec2 DestinationColor1;\n"
  "uniform float zoomLevel;\n"
  "uniform float verticalOffset;\n"
  "uniform float horizontalOffset;\n"
  "\n"
  "void main(void) {\n"
  "    gl_Position = vec4((vec2((Position1[0] + horizontalOffset), (Position1[1] + verticalOffset))),Position1[2], (1.0 / zoomLevel));\n"
  "    DestinationColor1 = SourceColor1 ;\n" 
  "}\n";

char* raster_fragment_shader_src =
  "precision highp float;\n"
  "uniform sampler2D texture;\n"
  "varying lowp vec2 DestinationColor1;\n"
  "\n"
  "void main(void) {\n"
  "    gl_FragColor = texture2D(texture, vec2(DestinationColor1[0], 1.0-DestinationColor1[1]));\n"
  "}\n";

  
  GLint simple_shader(GLint shader_type, char* shader_src) {
  GLint compile_success = 0;
  int shader_id = glCreateShader(shader_type);
  glShaderSource(shader_id, 1, &shader_src, 0);
  glCompileShader(shader_id);
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_success);
  if (compile_success == GL_FALSE) {
    GLchar message[256];
    glGetShaderInfoLog(shader_id, sizeof(message), 0, &message[0]);
    printf("glCompileShader Error: %s\n", message);
    exit(1);
  }

  return shader_id;
}


int simple_program(char* vertex_shader_src, char* fragment_shader_src) {

  GLint link_success = 0;

  GLint program_id = glCreateProgram();
  GLint vertex_shader = simple_shader(GL_VERTEX_SHADER, vertex_shader_src);
  GLint fragment_shader = simple_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

  glAttachShader(program_id, vertex_shader);
  glAttachShader(program_id, fragment_shader);

  glLinkProgram(program_id);

  glGetProgramiv(program_id, GL_LINK_STATUS, &link_success);

  if (link_success == GL_FALSE) {
    GLchar message[256];
    glGetProgramInfoLog(program_id, sizeof(message), 0, &message[0]);
    printf("glLinkProgram Error: %s\n", message);
    exit(1);
  }

  return program_id;
}

static void keyAction(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (action == GLFW_RELEASE){
		return;
	}
        pthread_t thread;
	if (key == 90){			//z key
		zoom(1);
		/*HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)tweenIn, NULL, 0, NULL);*/
                pthread_create(&thread, NULL, &tweenIn, NULL);
	}else if (key == 88){	//x key
		zoom(-1);
		/*HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)tweenOut, NULL, 0, NULL);*/
                pthread_create(&thread, NULL, &tweenOut, NULL);
	}else if (key == 73){	//i key
		panVertical(-.1);
		/*HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)tweenPanUp, NULL, 0, NULL);*/
                pthread_create(&thread, NULL, &tweenPanUp, NULL);
	}else if (key ==74){    //j key
		panHorizontal(.1);
		/*HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)tweenPanLeft, NULL, 0, NULL);*/
                pthread_create(&thread, NULL, &tweenPanLeft, NULL);
	}else if (key ==75){    //k key
		panVertical(.1);
		/*HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)tweenPanDown, NULL, 0, NULL);*/
                pthread_create(&thread, NULL, &tweenPanDown, NULL);
	}else if (key ==76){	//l key
		panHorizontal(-.1);
		/*HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)tweenPanRight, NULL, 0, NULL);*/
                pthread_create(&thread, NULL, &tweenPanRight, NULL);
	}
}

void GLInitRasterGroup(rasterGroup* group){
	glUseProgram((group->program_id));
	glEnableVertexAttribArray(group->positionSlot);
	glEnableVertexAttribArray(group->colorSlot);
	glUniform1i((group->colorSlot), 0); // Good
	//printf("Initializing: %s\n",group->name);
	int i;
	for(i=0; i<(group->size);i++){
		//if (group->rasterObjects[i].textureImage->Data[0] != ((unsigned char)'0')){
		//printf(" - Initializing: %s\n", group->rasterObjects[i].name);
		// Create Buffer
		glGenBuffers(1, &(group->rasterObjects[i].vertexBuffer));
		// Map GL_ARRAY_BUFFER to this buffer
		glBindBuffer(GL_ARRAY_BUFFER, (group->rasterObjects[i].vertexBuffer));//++++++++++++++++++++++++++++++++++++++
		// Send the data
		glBufferData(GL_ARRAY_BUFFER, sizeof(RasterVertices), group->rasterObjects[i].vertices, GL_STATIC_DRAW);
		
		glGenBuffers(1, &(group->indexBuffer));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group->indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

		//Texture Coords
		glGenBuffers(1, &(group->rasterObjects[i].textureCoordsBuffer));
		glBindBuffer(GL_ARRAY_BUFFER, group->rasterObjects[i].textureCoordsBuffer);//++++++++++++++++++++++++++++++++++++++
		glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), (group->rasterObjects[i].textureCoords), GL_STATIC_DRAW);

		
			//Generate Texture
			glGenTextures(1,&(group->rasterObjects[i].textureBuffer)); // Good
			glBindTexture(GL_TEXTURE_2D, (group->rasterObjects[i].textureBuffer));//++++++++++++++++++++++++++++++++++++++


			glTexImage2D(
				GL_TEXTURE_2D,         // target
				0,                     // level, 0 = base, no mipmap,
				GL_RGBA,
				group->rasterObjects[i].textureImage->Width,     // width
				group->rasterObjects[i].textureImage->Height,    // height

				0,                     // border, always 0 in OpenGL ES
				GL_RGBA,
				GL_UNSIGNED_BYTE,      // type
				(group->rasterObjects[i].textureImage->Data)
			);
			
			//printf("%s\n",group->rasterObjects[i].textureImage->Data);

			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		//}
	}
}

void GLRenderRasterGroup(rasterGroup* group){
	
		glUseProgram((group->program_id));
		glUniform1i(group->colorSlot, 0);
		//printf("Rendering: %s\n",group->name);
		int i;
		
		for(i=0; i<(group->size);i++){
			//if((group->rasterObjects[i].textureImage->Data[0])!=((unsigned char)'0')){
			glBindBuffer(GL_ARRAY_BUFFER, (group->rasterObjects[i].vertexBuffer)); //ok
			glVertexAttribPointer((group->positionSlot),
								  3,
								  GL_FLOAT,
								  GL_FALSE,
								  sizeof(rasterVertex),
								  (void*)0);
			
			glBindBuffer(GL_ARRAY_BUFFER, group->rasterObjects[i].textureCoordsBuffer);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), group->rasterObjects[i].textureCoords, GL_STATIC_DRAW);
			
			//Texture

			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, (group->rasterObjects[i].textureBuffer));// Good
			glVertexAttribPointer(group->colorSlot,
								  2,
								  GL_FLOAT,
								  GL_FALSE,
								  sizeof(GL_FLOAT)*2,
								  (void*)0);

			glTexImage2D(
				GL_TEXTURE_2D,         // target
				0,                     // level, 0 = base, no mipmap,
				GL_RGBA,
				group->rasterObjects[i].textureImage->Width,     // width
				group->rasterObjects[i].textureImage->Height,    // height

				0,                     // border, always 0 in OpenGL ES
				GL_RGBA,
				GL_UNSIGNED_BYTE,      // type
				(group->rasterObjects[i].textureImage->Data)
			);
			
			glDrawElements(GL_TRIANGLES,
						   sizeof(Indices) / sizeof(GLubyte),
						   GL_UNSIGNED_BYTE, 0);
			//}
	}
}

SVImage* wrapGImage(gimage* image){
	SVImage* svimagetemp = calloc(1,sizeof(SVImage));
	svimagetemp->Width = image->width;
	svimagetemp->Height = image->height;
	svimagetemp->Format = GL_RGBA; //GL_RGBA Constant
	svimagetemp->BytesPerPixel = image->bytes_per_pixel;
	svimagetemp->Data = (image->pixel_data);
	return svimagetemp;
}
void updateLODTest(int numTiles, LevelOfDetail* lod){
	int i;
	for(i=0; i < numTiles; i++){
		lod->data[i] = (wrapGImage(&zoomin_down_uiimage));
	}

}

void updateRasterGroup(rasterGroup* group){
int i;
	for(i=0; i<(group->size);i++){
		group->rasterObjects[i].textureImage->Data = (wrapGImage(&zoomin_down_uiimage))->Data;
	}
}

void syncRasterGroupLOD(rasterGroup* group, LevelOfDetail* lod){
int i;
	for(i=0; i<(group->size);i++){
		group->rasterObjects[i].textureImage->Data = lod->data[i]->Data;
	}
}

rasterObject* createTiles(int numXTiles, int numYTiles,int zIndex, LevelOfDetail* lod){
	float tileWidth = (2.0/numXTiles);
	float tileHeight = (2.0/numYTiles);
	int numTiles = (numXTiles * numYTiles);
	rasterObject* tiles = (calloc(numTiles,sizeof(rasterObject)));
	int i;
	int j;
	for (i = 0; i<numYTiles; i++){
		for(j = 0; j<numXTiles; j++){
			//sprintf(tiles[(i*numXTiles)+j].name, "Raster %i\0", ((i*numXTiles)+j));
			tiles[(i*numXTiles)+j].vertices[0] = (rasterVertex){ (((j*tileWidth)+tileWidth)-1) , ((i*tileHeight)-1) , zIndex};	//top right
			tiles[(i*numXTiles)+j].vertices[1] = (rasterVertex){ (((j*tileWidth)+tileWidth)-1) , (((i*tileHeight)+tileHeight)-1) , zIndex};	//bottom right
			tiles[(i*numXTiles)+j].vertices[2] = (rasterVertex){ ((j*tileWidth)-1) , (((i*tileHeight) + tileHeight)-1) , zIndex};	//bottom left
			tiles[(i*numXTiles)+j].vertices[3] = (rasterVertex){ ((j*tileWidth)-1) , ((i*tileHeight)-1) , zIndex};	//top left
			tiles[(i*numXTiles)+j].textureCoords = squareTexCoords;
			//tiles[(i*numXTiles)+j].textureImage = (wrapGImage(&openfile_up_uiimage));
			tiles[(i*numXTiles)+j].textureImage = (lod->data[(i*numXTiles)+j]);
			(lod->data[(i*numXTiles)+j]->Sampled) = 0;
		}
		//printf("Instantiated row %i\n",i);
	}
	return tiles;
}

void linkVisibleTiles(int numXTiles, int numYTiles, LevelOfDetail* lod){
	int* xLow = malloc(sizeof(int));
	int* xHigh = malloc(sizeof(int));
	int* yLow = malloc(sizeof(int));
	int* yHigh = malloc(sizeof(int));
	
	svGetVisibleTiles(xLow, xHigh, yLow, yHigh);
	int i;
	int j;
	for (i = 0; i<numYTiles; i++){
		if((i >= *yLow) && (i <= *yHigh)){
			for(j = 0; j<numXTiles;j++){
				if((j >= *xLow) && (j <= *xHigh)){
					resampleRasterGroup->rasterObjects[((i * numXTiles) + j)].textureImage = lod->data[((i * numXTiles) + j)];
				}
			}
		}
	}
	
}

void resample(){
	int numXBlocks = 24;
	int numYBlocks = 12;
	int xBlockSize = 1920;
	int yBlockSize = 1920;
	scale = lodArray[currentLOD-1]->scale;
	if(resampled == 0){
		printf("Level of Detail: %i\n",(currentLOD));
		resampleRasterGroup->rasterObjects = (createTiles(numXBlocks,numYBlocks,0,lodArray[(currentLOD)]));
		callocVisibleTiles(numXBlocks, numYBlocks, ((xBlockSize)*(yBlockSize)), 3, lodArray[(currentLOD)]);
		GLInitRasterGroup(resampleRasterGroup);
	}else{
		callocVisibleTiles(numXBlocks, numYBlocks, ((xBlockSize)*(yBlockSize)), 3, lodArray[(currentLOD)]);
		linkVisibleTiles(numXBlocks, numYBlocks, lodArray[(currentLOD)]);
	}
	int threadID;
	currentlySampling = 1;
        pthread_t thread;
        pthread_create(&thread, NULL, &sample, lodArray[currentLOD]);
    /*HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sample, lodArray[currentLOD], 0, &threadID);*/
	resampled = 1;
}

static void error_callback(int error, const char* description) {
  fputs(description, stderr);
}

void zoom(float direction){
	dZoomLevel += direction * dZoomLevel/10;
	int temp = currentLOD;
	//TODO
	if (!currentlySampling){
		currentLOD = getCurrentLOD();
		if (temp != currentLOD){
			printf("Level of detail changed!\n");
			resample();
		}
	}
}

void panVertical(float direction){
	dVerticalOffset += (direction/zoomLevel);
	if(dVerticalOffset > 1){
		dVerticalOffset = 1;
	}else if(dVerticalOffset < -1){
		dVerticalOffset = -1;
	}
}

void panHorizontal(float direction){
	dHorizontalOffset += (direction/zoomLevel);
	
	if(dHorizontalOffset > 1){
		dHorizontalOffset = 1;
	}else if(dHorizontalOffset < -1){
		dHorizontalOffset = -1;
	}
}

int getCurrentLOD(){
	int current;
	int nLevel = 1;
	while(dZoomLevel > nLevel){
		nLevel = nLevel << 1;
	}
	nLevel = log2(nLevel);
	current = (numLevels - (nLevel));
	if (current < 1){
		current = 1;
	}
	if (current >= numLevels){
		current = numLevels -1;
	}
	return (current);
}

void svGetVisibleTiles(int* xLowOut, int* xHighOut, int* yLowOut, int* yHighOut){
	
	if(currentLOD == numLevels){
		*xLowOut= 0;
		*xHighOut = -1;
		*yLowOut = 0;
		*yHighOut = -1;
		return;
	}else{
	
	int xSpan;
	int ySpan;
	int redux=1;
	int xLow = 0;
	int yLow = 0;
	unsigned int xHigh;
	unsigned int yHigh;
	float temp;
	int viewportScale= 1;
	float adjustedXCoord;
	float adjustedYCoord;
	float viewportWidth = 2.0;
	float viewportHeight = 2.0;
	float vpXOffset = 0.0;
	float vpYOffsetset = 0.0;
	double tileWidth;
	double tileHeight;
	
	//TODO get tile information
	int numXTiles = 24;
	int numYTiles = 12;
	
	viewportScale = viewportScale << (numLevels - (currentLOD +1));
	printf("DZoom level : %f\n", dZoomLevel);
	printf("viewportScale: %i\n",viewportScale);
	
	viewportWidth = (viewportWidth / viewportScale);
	viewportHeight = (viewportHeight / viewportScale);
	
	vpXOffset = (vpXOffset - (viewportWidth/2));
	vpYOffsetset = (vpYOffsetset - (viewportHeight/2));
	
	vpXOffset = (vpXOffset - dHorizontalOffset);
	vpYOffsetset = (vpYOffsetset + dVerticalOffset);
	
	tileWidth = (2.0/numXTiles);
	tileHeight = (2.0/numYTiles);
	
	printf("tileWidth = %ld\n", tileWidth);
	
	float xMap = (vpXOffset + 1.000001);
	xMap = (xMap / tileWidth);
	printf("XMAP : %f \n",xMap);
	xLow = (int)((int)(xMap));
	yLow = (((vpYOffsetset + 1)/tileHeight));
	
	printf("Adjusted coordinates: \n\t x:  %f \n\t y: %f\n", vpXOffset, vpYOffsetset);
	printf("Proposed xLow = %i\n Proposed yLow = %i\n",xLow,yLow);

	xSpan = numXTiles;
	ySpan = numYTiles;
	
	
	printf("current lod : %i\n", currentLOD);
	redux = redux << (numLevels - (currentLOD + 1));
	printf("Redux : %i \n\n", redux);
	temp = (float)((float)xSpan/(float)redux);
	//xSpan = (ceil((xSpan/redux)));
	xSpan = ceil(temp);
	temp = (float)((float)ySpan/(float)redux);
	ySpan = ceil(temp);
	
	xHigh = xLow + xSpan;
	yHigh = yLow + ySpan;
	
	
	if(xHigh > numXTiles){
		xHigh = numXTiles;
	}
	if(yHigh > numYTiles){
		yHigh = numYTiles;
	}
	if (xLow < 0){
		xLow = 0;
	}
	if (yLow < 0 ){
		yLow = 0;
	}
	
	if(currentLOD == (numLevels -1)){
		xHigh = numXTiles;
		yHigh = numYTiles;
		xLow = 0;
		yLow = 0;
	}
	
	//printf("xLow: %i\n xHigh: %i\n yLow: %i\n yHigh: %i\n", xLow, xHigh, yLow, yHigh);
	
	*xLowOut = xLow;
	*xHighOut =xHigh;
	*yLowOut= yLow;
	*yHighOut = yHigh;
	}
}

void callocVisibleTiles(int numXTiles, int numYTiles, int originalBlockSize, int bandCount, LevelOfDetail* lod){
	int* xLow = malloc(sizeof(int));
	int* xHigh = malloc(sizeof(int));
	int* yLow = malloc(sizeof(int));
	int* yHigh = malloc(sizeof(int));
	
	svGetVisibleTiles(xLow, xHigh, yLow, yHigh);
	
	if ((*xHigh < 0) || (*xHigh < *xLow)){
		*xHigh = numXTiles;
	}
	if ((*yHigh < 0)|| (*yHigh < *yLow)){
		*yHigh = numYTiles;
	}
	
	int i;
	int j;
	
	printf("xLow: %i \n xHigh: %i \n yLow: %i\n yHigh: %i\n", *xLow, *xHigh, *yLow, *yHigh);
	
	for (i = 0; i<numYTiles; i++){
		if((i >= *yLow) && (i < *yHigh)){
			for(j = 0; j<numXTiles;j++){
				if((j >= *xLow) && (j < *xHigh)){
					if (lod->data[((i * numXTiles) + j)]->Sampled ==0){
						lod->data[((i * numXTiles) + j)]->Data = calloc(((originalBlockSize/(scale*scale))*(bandCount+1)),sizeof(unsigned char));
					}
				}
			}
		}
	}
}

void checkParams(int argc, char **argv) {
    if(argc > 1) {
        filepath = argv[1];
    }
}

int main(int argc, char **argv) {
    checkParams(argc, argv);
    printf("%s\n", filepath);
  
   GLuint index_buffer;

  glfwSetErrorCallback(error_callback);
  // Initialize GLFW library
  if (!glfwInit())
    return -1;
	
  //Resolution information
  const GLFWvidmode* res;
  res = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int resHeight = res->height;
  int resWidth = res->width;
	
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow(resWidth,
                            resHeight,
                            "SciViewer",
							NULL,
                            NULL);

 if (!window) {
    glfwTerminate();
    printf("glfwCreateWindow Error\n");
    exit(1);
  }
  
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyAction);

  
   /****************************/

  numLevels = getNumLevels();
	GDALDatasetH  hDataset;
	GDALAllRegister();
	GDALRasterBandH hBand;
	hDataset = GDALOpen( filepath, GA_ReadOnly );
    if( hDataset == NULL )
    {
    	printf("Unable to open file."); 
		exit(0);
    }

	int origWidth = GDALGetRasterXSize( hDataset ); 
	int origHeight = GDALGetRasterYSize( hDataset ); 

	hBand = GDALGetRasterBand( hDataset, 1 );

	int* xBlockSize = malloc (sizeof(int));
	int* yBlockSize = malloc (sizeof(int));
	
	int origBandCount = GDALGetRasterCount( hDataset );
	GDALClose(hDataset);
	//get block size
	GDALGetBlockSize( hBand, xBlockSize, yBlockSize);

	int numXBlocks = origWidth/(*xBlockSize);
	int numYBlocks = origHeight/(*yBlockSize);

	//TODO get from GDAL
	int numTiles = numXBlocks*numYBlocks;
	int i;
	
	//initialize array of levels of detail
	LevelOfDetail** mLod;
	
	mLod = initLODArray(numLevels, numTiles);
	scale = mLod[(currentLOD-1)]->scale;
	
	lodArray = mLod;
	
	int xOutputSize = ((*xBlockSize) / scale);
	int yOutputSize = ((*yBlockSize) / scale);

  //Original Sample Raster Group

  rasterGroup firstRasterGroup;
  firstRasterGroup.name = "Original Sample Raster Group\0";
  
  firstRasterGroup.size = numTiles;
  
  firstRasterGroup.program_id = simple_program(raster_vertex_shader_src, raster_fragment_shader_src);
  firstRasterGroup.positionSlot = glGetAttribLocation(firstRasterGroup.program_id, "Position1");
  firstRasterGroup.colorSlot = glGetAttribLocation(firstRasterGroup.program_id, "SourceColor1");
  
  glEnableVertexAttribArray(firstRasterGroup.positionSlot);
  glEnableVertexAttribArray(firstRasterGroup.colorSlot); 
  

  firstRasterGroup.rasterObjects = (createTiles(numXBlocks,numYBlocks,1,mLod[(currentLOD-1)]));
  callocVisibleTiles(numXBlocks, numYBlocks, ((*xBlockSize)*(*yBlockSize)), origBandCount, mLod[currentLOD-1]);
  
  
  initialRasterGroup = &firstRasterGroup;
  
  GLint zl = glGetUniformLocation(firstRasterGroup.program_id, "zoomLevel");
  glUniform1f(zl, zoomLevel);
  GLint pv = glGetUniformLocation(firstRasterGroup.program_id, "verticalOffset");
  glUniform1f(pv, verticalOffset);
  GLint ph = glGetUniformLocation(firstRasterGroup.program_id, "horizontalOffset");
  glUniform1f(ph, horizontalOffset);
  
    GLInitRasterGroup(&firstRasterGroup);	
  /////////////////////RESAMPLE/////////////////////////////
  
  rasterGroup secondRasterGroup;
  secondRasterGroup.name = "Resample Sample Raster Group\0";
  
  secondRasterGroup.size = numTiles;
  
  secondRasterGroup.program_id = firstRasterGroup.program_id;
  
  secondRasterGroup.positionSlot = glGetAttribLocation(firstRasterGroup.program_id, "Position1");
  secondRasterGroup.colorSlot = glGetAttribLocation(firstRasterGroup.program_id, "SourceColor1");
  /*
  GLint rzl = glGetUniformLocation(first.program_id, "zoomLevel");
  glUniform1f(rzl, zoomLevel);
  GLint rpv = glGetUniformLocation(secondRasterGroup.program_id, "verticalOffset");
  glUniform1f(rpv, verticalOffset);
  GLint rph = glGetUniformLocation(secondRasterGroup.program_id, "horizontalOffset");
  glUniform1f(rph, horizontalOffset);
*/
  glEnableVertexAttribArray(secondRasterGroup.positionSlot);
  glEnableVertexAttribArray(secondRasterGroup.colorSlot); 
  
  resampleRasterGroup = &secondRasterGroup;
  

  int threadID;
	
	currentlySampling = 1;
        pthread_t thread;
        pthread_create(&thread, NULL, &sample, mLod[currentLOD-1]);
  /*HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sample, mLod[currentLOD-1], 0, &threadID);*/
 
  //Todo Create Function to init index buffer

  //---------------------------------------------------------------------------------------
  
  
    
  
  while (!glfwWindowShouldClose(window)) {
    //syncRasterGroupLOD(&firstRasterGroup,mLod[(currentLOD-1)]);
    glUniform1f(zl, zoomLevel);
	glUniform1f(pv, verticalOffset);
	glUniform1f(ph, horizontalOffset);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
	
	int* winwidth = malloc(sizeof(int));
	int* winheight = malloc(sizeof(int));
	glfwGetWindowSize(window, winwidth, winheight);
	
    glViewport(0, 0, *winwidth, *winheight);
    //GLInitRasterGroup(&firstRasterGroup);	
    //GLInitRasterGroup(&firstRasterGroup);	
	
	GLRenderRasterGroup(&firstRasterGroup);
	if (resampled !=0){
		GLRenderRasterGroup(&secondRasterGroup);
	}
	
	
	glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
