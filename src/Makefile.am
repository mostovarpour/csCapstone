bin_PROGRAMS = sciviewer
sciviewer_SOURCES = main.c raster.c glhelper.c gdalreader.c args.c
AM_CFLAGS = -std=c99 -I./headers
#AM_LDFLAGS = -lgdal -lGL -lglfw3 -lpthread -lGLEW
#Get dependencies
if DARWIN
LDADD = -lglfw3 -lgdal -lGLEW -lpthread
AM_LDFLAGS = -framework OpenGL
else
LDADD = -lglfw3 -lgdal -lGLEW -lGLU -lGL -lX11 -lpthread -ldl -lm -lXxf86vm -lXrandr -lXi -lXcursor -lXinerama
endif
