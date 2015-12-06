all:
	gcc -c lod2.c -o lod2.o
	gcc -c prob.c -o prob.o
	gcc -c main.c -o main.o
	gcc main.o lod2.o prob.o -w -o main -lglfw3 -lgdal -lGL -lX11 -lpthread -ldl -lm -lXxf86vm -lXrandr -lXi -lXcursor -lXinerama
