OBJDIR=build/obj
BINDIR=build/bin
LIBS=glfw3 gdal GLEW GLU GL X11 pthread dl m Xxf86vm Xrandr Xi Xcursor Xinerama
LINK=$(foreach lib, $(LIBS), -l$(lib))

all: polygons

clean_polygons:
	rm -f polygons
polygons: clean_polygons
	gcc -c opengl_polygons.c -Iheaders -o opengl_polygons.o
	gcc -c glhelper.c -Iheaders -o glhelper.o
	gcc opengl_polygons.o glhelper.o -o polygons $(LINK)
	rm glhelper.o
	rm opengl_polygons.o
