#include "args.h"

void args_set_width(char *width)
{
    int w = atoi(width);
    if(w <= 10)
    {
        puts("Invalid width");
        exit(EXIT_FAILURE);
    }
    screen_width = w;
}

void args_set_height(char *height)
{
    int h = atoi(height);
    if(h <= 10)
    {
        puts("Invalid height");
        exit(EXIT_FAILURE);
    }
    screen_height = h;
}

void check_parameters(int argc, char **argv)
{
    // check if there is an even number of arguments
    if(((argc - 1) % 2) != 0) // (argc - 1) to ignore the command name as an argument
    {
        puts("ERROR: missing argument");
        args_print_usage();
        exit(EXIT_FAILURE);
    }
    // check parameter list
    for(int i = 1; i < argc; i+=2) // add 2 each time to only check the flags
    {
        if(strcmp("-w", argv[i]) == 0)
            args_set_width(argv[i+1]);
    }
}
