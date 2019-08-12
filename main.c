#include <stdio.h>
#include <stdbool.h>

#include "SDL2/SDL.h"

#include "world.h"
#include "tetromino.h"

#define FPS 60
#define NORMAL_SPEED 800
#define FAST_SPEED 50
#define HORIZONTAL_SPEED 150

#define WINDOW_WIDTH GAME_WIDTH*TILE_SIZE
#define WINDOW_HEIGHT GAME_HEIGHT*TILE_SIZE

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* tileTexture = NULL;

void initGame();
int exitGame();

SDL_Texture* loadTexture(char* path);
void capFramerate(Uint32 startTick);

int main()
{
    initGame();
    Tetromino* tetro = spawnTetromino();

    Uint32 startFrameTick;
    Uint32 vMoveTick = SDL_GetTicks();
    Uint32 hMoveTick = SDL_GetTicks();
    int hMove = -1;
    bool keyDown = false;
    bool fastSpeed = false;
    bool moveTetro = false;

    while (1) {
        startFrameTick = SDL_GetTicks();

        // Downward movement 
        if (!fastSpeed && 
                SDL_GetTicks() - vMoveTick >= NORMAL_SPEED)
            moveTetro = true;
        else if (fastSpeed && 
                SDL_GetTicks() - vMoveTick >= FAST_SPEED)
            moveTetro = true;

        /*
        if (moveTetro) {
            moveTetromino(tetro, DOWN);
            vMoveTick = SDL_GetTicks();
            moveTetro = false;
        }
        */

        // Horizontal movement
        if (SDL_GetTicks() - hMoveTick >= HORIZONTAL_SPEED) {
            if (hMove != -1) {
                moveTetromino(tetro, hMove, 1);
                hMoveTick = SDL_GetTicks();
            }
        }

        // Key hold press handling
        fastSpeed = false;
        Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_DOWN])
            fastSpeed = true;

        if ((state[SDL_SCANCODE_LEFT] && state[SDL_SCANCODE_RIGHT]) 
                || (!state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT]))
            hMove = -1;
        else if (state[SDL_SCANCODE_LEFT])
            hMove = LEFT;
        else if (state[SDL_SCANCODE_RIGHT])
            hMove = RIGHT;

        // Event handling
        SDL_Event event;
        while(SDL_PollEvent(&event)) {

            if(event.type == SDL_QUIT)
                return exitGame();

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_UP)
                    moveTetromino(tetro, UP, 1);
                if (event.key.keysym.sym == SDLK_SPACE)
                    rotateTetromino(tetro);
                if (event.key.keysym.sym == SDLK_DOWN)
                    moveTetromino(tetro, DOWN, 1);
            }

        }

        checkForFilledRow();

        SDL_RenderClear(renderer);
        renderWorld(renderer, tileTexture);
        renderTetromino(tetro, renderer, tileTexture);
        SDL_RenderPresent(renderer);

        capFramerate(startFrameTick);
    }

}

void initGame()
{
    srand(time(0));
    SDL_Init(SDL_INIT_EVERYTHING); 

    window = SDL_CreateWindow("MUh Title",
            800, 800,
            WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    tileTexture = loadTexture("tiles.bmp");
}

int exitGame()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void capFramerate(Uint32 startTick)
{
    if ((1000 / FPS) > SDL_GetTicks() - startTick)
        SDL_Delay( 1000 / FPS - (SDL_GetTicks() - startTick));
}

SDL_Texture* loadTexture(char* path)
{
    SDL_Surface* s = SDL_LoadBMP(path);
    if (s == NULL)
        printf("Failed to laod surface\n");
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    if (t == NULL)
        printf("Failed to generate texture");

    return t;
}
