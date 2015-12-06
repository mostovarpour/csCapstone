OBJDIR=build/obj
BINDIR=build/bin
LIBS=glfw3 gdal GL X11 pthread dl m Xxf86vm Xrandr Xi Xcursor Xinerama

LINK=$(foreach lib, $(LIBS), -l$(lib))
all: clean
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)
	gcc -c lod2.c -o $(OBJDIR)/lod2.o
	gcc -c prob.c -o $(OBJDIR)/prob.o
	gcc -c main.c -o $(OBJDIR)/main.o
	gcc $(OBJDIR)/main.o $(OBJDIR)/lod2.o $(OBJDIR)/prob.o -w -o $(BINDIR)/main $(LINK)

clean:
	rm -r build
	
