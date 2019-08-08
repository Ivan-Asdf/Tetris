#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"

#define window_width 800
#define window_height 600

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING); 

    SDL_Window* window;
    window = SDL_CreateWindow("MUh Title",
                              0, 0,
                              window_width, window_height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    while (1) {
        Uint32 color = SDL_MapRGB(surface->format, rand() % 255, rand() % 255,
                rand() % 255);
        SDL_FillRect(surface, NULL, color);
        SDL_UpdateWindowSurface(window);

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                SDL_SaveBMP(surface, "art.bmp");
                SDL_DestroyWindow(window);
                SDL_Quit();
            }
        }
    }
}
