// Include windows for CreateThread or pthread for linux threads depending on build platform
#if defined _WIN32 || _WIN64
    #include <Windows.h>
#else
    #include <pthread.h>
#endif
// boolean type
typedef unsigned char bool;
#define false 0
#define true 1

//These two ifdefs are for compiling on OSX
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
