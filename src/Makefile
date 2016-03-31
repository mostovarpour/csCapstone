OBJDIR=build/obj
BINDIR=build/bin

platform=`uname`

OS:= $(shell uname)
ifeq ($(OS), Darwin)
	LIBS=glfw3 gdal GLEW pthread 
	MACLINK= -framework OpenGL
else
	LIBS=glfw3 gdal GLEW GLU GL glut IL X11 pthread dl m Xxf86vm Xrandr Xi Xcursor Xinerama png 
endif
LINK=$(foreach lib, $(LIBS), -l$(lib))

FILES=glhelper main gdalreader raster args

all: clean setup compile
	g++ $(foreach file, $(FILES), $(OBJDIR)/$(file).o ) -w -o $(BINDIR)/main $(LINK) $(MACLINK)

setup:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)

clean:
	rm -rf build
	
compile:
	$(foreach file, $(FILES), g++ -c -std=c++11 -Iheaders $(file).c -o $(OBJDIR)/$(file).o;)
