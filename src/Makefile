OBJDIR=build/obj
BINDIR=build/bin

platform=`uname`

OS:= $(shell uname)
ifeq ($(OS), Darwin)
	LIBS=glfw3 gdal GLEW pthread 
	MACLINK= -framework OpenGL
else
	LIBS=glfw3 gdal GLEW GLU GL X11 pthread dl m Xxf86vm Xrandr Xi Xcursor Xinerama
endif
LINK=$(foreach lib, $(LIBS), -l$(lib))

FILES=glhelper main gdalreader raster args

all: clean setup compile
	gcc $(foreach file, $(FILES), $(OBJDIR)/$(file).o ) -w -o $(BINDIR)/main $(LINK) $(MACLINK)

#osx:
	#$(foreach file, $(files), gcc -c -std=c99 -Iheaders -framework OpenGL -framework GLUT $(file).c -o $(OBJDIR)/$(file).o) 

setup:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)

clean:
	rm -rf build
	
compile:
	$(foreach file, $(FILES), gcc -c -std=c99 -Iheaders $(file).c -o $(OBJDIR)/$(file).o;)
