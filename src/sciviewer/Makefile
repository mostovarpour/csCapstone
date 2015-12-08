OBJDIR=build/obj
BINDIR=build/bin
LIBS=glfw3 gdal GL X11 pthread dl m Xxf86vm Xrandr Xi Xcursor Xinerama

LINK=$(foreach lib, $(LIBS), -l$(lib))
all: clean setup lod prob main tween
	gcc $(OBJDIR)/main.o $(OBJDIR)/lod2.o $(OBJDIR)/prob.o $(OBJDIR)/tween.o -w -o $(BINDIR)/main $(LINK)

prob2test: clean setup lod prob2 main tween
	gcc $(OBJDIR)/main.o $(OBJDIR)/lod2.o $(OBJDIR)/prob.o $(OBJDIR)/tween.o -w -o $(BINDIR)/main $(LINK)

setup:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)

lod:
	gcc -c lod2.c -o $(OBJDIR)/lod2.o

prob:
	gcc -c prob.c -o $(OBJDIR)/prob.o

prob2:
	gcc -c prob2.c -o $(OBJDIR)/prob.o

main:
	gcc -c main.c -o $(OBJDIR)/main.o

tween:
	gcc -c tween.c -o $(OBJDIR)/tween.o

clean:
	rm -rf build
	
