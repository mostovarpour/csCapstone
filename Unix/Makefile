# TL;DR; 
# Add the file name w/o .c to the FILES variable below and it should just work
# If it doesn't just work then let Daniel know after attempting to fix it.
PROGRAM_NAME = gdalViewer

# These are the files that will be compiled. If it needs any extra compiler flags then either add them
# the flags to CFLAGS or manually add a compiler call above the g++ linking also make sure it's added into
# the g++ linking call. Ask Daniel for help if needed
FILES = helloworld





OBJPATH = build/obj
SRCPATH = ../src

#These defines were used in the default bgfx makefile so I'm just repeating them here
DEFINES =  -DBX_CONFIG_ENABLE_MSVC_LEVEL4_WARNINGS=1 -D__STDC_LIMIT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS

#These flags were used in the default file, dunno what some of them are
#All the -W ones are warnings and that's all I know
CFLAGS =  -MMD -MP -Wall -Wextra -g -O3 -m64 -Wshadow -mfpmath=sse -msse2 -Wunused-value -Wundef -m64
INCLUDE = -I../bgfx/bx/include -I../bgfx/bgfx/include -I../bgfx/bgfx/3rdparty -I../bgfx/bgfx/examples/common 
COMPILE = gcc $(DEFINES) $(CFLAGS) $(INCLUDE)

# Flags for compiling and linking to our program
LIBFLAGS =  -L./lib -m64 -Wl,--gc-sections ./lib/libbgfx64.a ./lib/libcommon64.a -lrt -ldl -lX11 -lGL -lpthread
LINK = $(foreach object, $(FILES), $(OBJPATH)/$(object).o)

all:
	mkdir -p build/bin
	mkdir -p build/obj
	$(foreach file, $(FILES), $(COMPILE) -o $(OBJPATH)/$(file).o -c $(SRCPATH)/$(file).c)
	g++ -o ./build/bin/$(PROGRAM_NAME) $(LINK) $(LIBFLAGS)
