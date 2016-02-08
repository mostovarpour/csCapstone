AUTOMAKE_OPTIONS = subdir-objects
AM_CFLAGS = -std=c99 -O2 -Iheaders

bin_PROGRAMS = sciviewer
sciviewer_SOURCES = raster.c glhelper.c gdalreader.c args.c main.c ./headers/args.h ./headers/raster.h ./headers/glhelper.h ./headers/gdalreader.h ./headers/args.h
sciviewer_LDADD = -lglfw3 -lgdal
sciviewer_LDFLAGS = -lX11 -ldl -lm -lXxf86vm -Xi -Xinerama -Xcursor
