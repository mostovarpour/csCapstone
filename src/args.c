#include "args.h"

void args_set_width(char *width)
{
    int w = atoi(width);
    if(w <= ARGS_MINIMUM_WIDTH)
    {
        puts("Invalid width");
        exit(EXIT_FAILURE);
    }
    screen_width = w;
}

void args_set_height(char *height)
{
    int h = atoi(height);
    if(h <= ARGS_MINIMUM_HEIGHT)
    {
        puts("Invalid height");
        exit(EXIT_FAILURE);
    }
    screen_height = h;
}

void args_check(int argc, char **argv)
{
    file_path = argv[argc-1]; // filepath should be the last argument

	for (int i = 0; i < argc; i++)
	{
		if (strcmp("-w", argv[i]) == 0) args_set_width(argv[i++ + 1]);
		else if (strcmp("-h", argv[i]) == 0) args_set_height(argv[i++ + 1]);
	}
}

void args_print_usage()
{
    puts("Usage: sciviewer [OPTIONS] <image-path>");
    puts("");
    puts("Available options are:");
    puts("-w <number>          Set the width of the window");
    puts("-h <number>          Set the height of the window");
}
