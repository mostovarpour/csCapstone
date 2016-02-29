OBJDIR=build/obj
BINDIR=build/bin

platform=`uname`

OS:= $(shell uname)
ifeq ($(OS), Darwin)
	LIBS=glfw3 gdal GLEW pthread 
	MACLINK= -framework OpenGL
else
	LIBS=GL X11 pthread
endif

LINK=-L/home/matthew/software/bgfx/.build/linux64_gcc/bin -lbgfxRelease -lexample-commonRelease $(foreach lib, $(LIBS), -l$(lib))

FILES=main.cpp

all: clean setup compile
	g++ $(foreach file, $(FILES), $(OBJDIR)/$(file).o ) -w -o $(BINDIR)/main $(LINK) $(MACLINK)

setup:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)

clean:
	rm -rf build
	
compile:
	$(foreach file, $(FILES), g++ -c -std=c++11 -Iheaders -I/home/matthew/software/bgfx/include -I/home/matthew/software/bx/include -I/home/matthew/software/bgfx/examples/common -I/home/matthew/software/bgfx/examples/00-helloworld $(file) -o $(OBJDIR)/$(file).o;)
