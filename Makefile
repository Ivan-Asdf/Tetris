config=$(shell sdl2-config --static-libs)

a.out : main.c tetromino.h tetromino.c world.h world.c wallkick.h
	gcc -g main.c tetromino.c world.c -lSDL2

static : main.c tetromino.h tetromino.c world.h world.c wallkick.h
	gcc -g main.c tetromino.c world.c -static $(shell sdl2-config --static-libs)

