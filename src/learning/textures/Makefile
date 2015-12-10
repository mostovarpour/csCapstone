OBJDIR=build/obj
BINDIR=build/bin
LIBS=glfw3 gdal GLEW GLU GL X11 pthread dl m Xxf86vm Xrandr Xi Xcursor Xinerama
LINK=$(foreach lib, $(LIBS), -l$(lib))

all: textures 

clean_textures:
	rm -f textures 
textures: clean_textures
	gcc -c opengl_texturing.c -Iheaders -o opengl_texturing.o
	gcc -c glhelper.c -Iheaders -o glhelper.o
	gcc opengl_texturing.o glhelper.o -o textures $(LINK)
	rm glhelper.o
	rm opengl_texturing.o
