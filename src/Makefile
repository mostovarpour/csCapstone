OBJDIR=build/obj
BINDIR=build/bin
SHADERDIR=build/bin/shaders/glsl

platform=`uname`

OS:= $(shell uname)
ifeq ($(OS), Darwin)
	LIBS=glfw3 gdal GLEW pthread 
	MACLINK= -framework OpenGL
else
	LIBS=GL X11 pthread
endif

LINK=-L../bgfx/.build/linux64_gcc/bin -lbgfxRelease -lexample-commonRelease -lgdal $(foreach lib, $(LIBS), -l$(lib))

FILES=helloworld.cpp

all: clean setup compile
	g++ $(foreach file, $(FILES), $(OBJDIR)/$(file).o ) -w -o $(BINDIR)/main $(LINK) $(MACLINK)

setup:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)
	mkdir -p $(SHADERDIR)

clean:
	rm -rf build
	
compile:
	../bgfx/tools/bin/linux/shaderc -f frag_shader.sc -o frag_shader.bin --type f --platform linux
	../bgfx/tools/bin/linux/shaderc -f vertex_shader.sc -o vertex_shader.bin --type v --platform linux
	mv vertex_shader.bin $(SHADERDIR)
	mv frag_shader.bin $(SHADERDIR)
	$(foreach file, $(FILES), g++ -c -std=c++11 -Iheaders -I../bgfx/include -I../bx/include -I../bgfx/examples/common -I../bgfx/examples/00-helloworld $(file) -o $(OBJDIR)/$(file).o;)
