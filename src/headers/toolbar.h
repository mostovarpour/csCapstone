#ifndef __TOOLBAR_H_
#define __TOOLBAR_H_
#include "os.h"
#define ILUT_USE_OPENGL //Must be defined before calling the devIL headers
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
int LoadImage(char *filename); 
void display();
#endif
