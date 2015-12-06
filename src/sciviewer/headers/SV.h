#ifndef __SV_IMAGE__
#define __SV_IMAGE__
#include <GLES2/gl2.h>
typedef struct {
	int Width, Height; // Image dimensions
	int Format; // OpenGL pixel format
	int BytesPerPixel; // Number of bytes per pixel
	int Sampled;
	unsigned char *Data; // Pointer to pixel data
} SVImage;

typedef struct {
  float position[3];
} rasterVertex;

typedef struct {
  char* name;
  GLuint vertexBuffer;
  //GLuint texture;
  GLuint textureBuffer;
  GLuint textureCoordsBuffer;
  const GLfloat* textureCoords;
  rasterVertex vertices[4];
  SVImage* textureImage;
} rasterObject;

typedef struct {
  char* name;
  GLint program_id;
  GLint positionSlot;
  GLint colorSlot;
  GLuint indexBuffer;
  int size;
  rasterObject* rasterObjects;
} rasterGroup;

typedef struct{
	int scale;
	SVImage **data;
} LevelOfDetail;


/*Function prototypes */
void zoom(float direction);
void panHorizontal(float direction);
void panVertical(float direction);
void svGetVisibleTiles(int* xLowOut, int* xHighOut, int* yLowOut, int* yHighOut);
void GLInitRasterGroup(rasterGroup* group);
void callocVisibleTiles(int numXTiles, int numYTiles, int originalBlockSize, int bandCount, LevelOfDetail* lod);
#endif
