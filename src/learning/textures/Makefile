OBJDIR=build/obj
BINDIR=build/bin
LIBS=glfw3 gdal GLEW GLU GL X11 pthread dl m Xxf86vm Xrandr Xi Xcursor Xinerama
LINK=$(foreach lib, $(LIBS), -l$(lib))
FILES=opengl_texturing glhelper gdalreader
COMPILE=$(foreach file, $(FILES), gcc -Iheaders -c $(file).c -o $(file).o; )
BUILD=gcc $(foreach file, $(FILES), $(file).o ) -o textures $(LINK)

all: textures 

clean_textures:
	rm -f textures 
textures: clean_textures
	$(COMPILE)
	$(BUILD)
	rm glhelper.o
	rm opengl_texturing.o
	rm gdalreader.o
