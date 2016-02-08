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
LINK=-L../bgfx/.build/linux64_gcc/bin -lbgfxRelease -lexample-commonRelease $(foreach lib, $(LIBS), -l$(lib))

FILES=main.cpp

all: clean setup compile
	g++ $(foreach file, $(FILES), $(OBJDIR)/$(file).o ) -w -o $(BINDIR)/main $(LINK) $(MACLINK)

setup:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)

clean:
	rm -rf build
	
compile:
	$(foreach file, $(FILES), g++ -c -std=c++11 -Iheaders -I../bgfx/include -I../bx/include -I../bgfx/examples/common $(file) -o $(OBJDIR)/$(file).o;)
