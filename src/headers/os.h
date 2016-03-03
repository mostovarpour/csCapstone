#ifndef __OS__
#define __OS__

// Include windows for CreateThread or pthread for linux threads depending on build platform
#if defined _WIN32 || _WIN64
    #include <Windows.h>
	typedef unsigned char bool;
#else
    #include <pthread.h>
#endif
// boolean type
#define false 0
#define true 1

//This is for compiling on OSX
#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h>
//#include <OpenGL/glu.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
//#include <GL/glu.h>
#endif
#endif
