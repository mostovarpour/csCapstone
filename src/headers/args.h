#ifndef __ARGS_H__
#define __ARGS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void args_check(int argc, char **argv);
void args_print_usage();
extern int screen_width;  // declared in main.c
extern int screen_height; // declared in main.c
#endif
