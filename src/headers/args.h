#ifndef __ARGS_H__
#define __ARGS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
#define ARGS_MINIMUM_WIDTH 10
#define ARGS_MINIMUM_HEIGHT 10
#define ARGS_MINIMUM_SAMPLING_RATE 10
void args_check(int argc, char **argv);
void args_print_usage();
extern int screen_width;  // declared in main.c
extern int screen_height; // declared in main.c
extern int sample_rate;   // declared in main.c
extern char* file_path;   // declared in main.c
#endif
