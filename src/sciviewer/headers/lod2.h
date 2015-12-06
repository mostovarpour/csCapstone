#ifndef __LOD_H__
#define __LOD_H__

#include <stdlib.h>
#include <stdio.h>
#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>
#include "SV.h"
#include "gdal.h"
#include "cpl_conv.h"
void initLevelOfDetail(int tiles, LevelOfDetail* lod);
int getNumLevels();
void initLODSVImage(LevelOfDetail *lod, int numTiles);
extern char* filepath;
#endif
