LIBS=GL X11 pthread

all:
	g++ helloworld.cpp -I../../include -I../../../bx/include -I../common -L../../.build/linux64_gcc/bin -lbgfxRelease -lexample-commonRelease -lbgfx-shared-libRelease $(foreach lib, $(LIBS), -l$(lib))
