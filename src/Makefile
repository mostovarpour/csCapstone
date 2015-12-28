OBJDIR=build/obj
BINDIR=build/bin
LIBS=glfw3 gdal GLEW GLU GL X11 pthread dl m Xxf86vm Xrandr Xi Xcursor Xinerama
LINK=$(foreach lib, $(LIBS), -l$(lib))

FILES=glhelper main gdalreader raster

all: clean setup compile
	gcc $(foreach file, $(FILES), $(OBJDIR)/$(file).o ) -w -o $(BINDIR)/main $(LINK)

setup:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)

clean:
	rm -rf build
	
compile:
	$(foreach file, $(FILES), gcc -c -Iheaders $(file).c -o $(OBJDIR)/$(file).o;)
