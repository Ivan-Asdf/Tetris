# Static linking dependencies script
config=$(shell sdl2-config --static-libs)
win_config=$(shell win_sdl2/bin/sdl2-config --static-libs)

source = $(wildcard *.c)
header = $(wildcard *.h)

linux : $(source) $(header)
	gcc -g $(source) -lSDL2

# DOESNT WORK!!! aplay library has no static build. It is reported as a bug years ago still not fixed.
static : $(source) $(header)
	gcc -g $(source) -static $(shell sdl2-config --static-libs)

# Run make(mingw64) in bash(git bash, cygwin..)
windows: $(source) $(header)
	mkdir -p build 
	cp win_sdl2/bin/SDL2.dll build/SDL2.dll
	cp tiles.bmp build/tiles.bmp
	gcc -g $(source) -o build/tetris -Iwin_sdl2/include -Lwin_sdl2/lib -lmingw32 -lSDL2main -lSDL2

windows_static: $(source) $(header)
	mkdir -p build 
	cp tiles.bmp build/tiles.bmp
	gcc -static -g $(source) -o build/tetris -Iwin_sdl2/include -Lwin_sdl2/lib $(win_config)	
	


