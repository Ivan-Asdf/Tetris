#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include "SDL2/SDL.h"

#include "world.h"
#include "tetromino.h"

#define FPS 60
#define NORMAL_SPEED 800
#define FAST_SPEED 50
#define HORIZONTAL_SPEED 70
#define DAS_DELAY 250

#define WINDOW_WIDTH GAME_WIDTH*TILE_SIZE
#define WINDOW_HEIGHT GAME_HEIGHT*TILE_SIZE

bool debug = true;

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
    Tetromino* t = spawnTetromino();

    Uint32 startFrameTick;
    Uint32 vMoveTick = SDL_GetTicks();
    Uint32 hMoveTick = SDL_GetTicks();
    Uint32 dasDelayTick = ULONG_MAX;

    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;

    int xMove = 0;

    while (1) {
        startFrameTick = SDL_GetTicks();

        // Event handling
        SDL_Event event;
        while(SDL_PollEvent(&event)) {

            if(event.type == SDL_QUIT)
                return exitGame();
            else if (event.type == SDL_KEYDOWN) {
                if(event.key.repeat == 0) {
                    if (event.key.keysym.sym == SDLK_UP)
                        upPressed = true;
                    if (event.key.keysym.sym == SDLK_DOWN) {
                        downPressed = true;
                    }
                    if (event.key.keysym.sym == SDLK_LEFT) {
                        leftPressed = true;
                        moveTetromino(t, -1, 0);
                    }
                    if (event.key.keysym.sym == SDLK_RIGHT) {
                        rightPressed = true;
                        moveTetromino(t, 1, 0);
                    }
                    if(leftPressed || rightPressed)
                        dasDelayTick = SDL_GetTicks();

                    if (event.key.keysym.sym == SDLK_x)
                        rotateTetromino(t, true);
                    else if (event.key.keysym.sym == SDLK_z)
                        rotateTetromino(t, false);
                }
            }
            else if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_UP)
                    upPressed = false;
                if (event.key.keysym.sym == SDLK_DOWN)
                    downPressed = false;
                if (event.key.keysym.sym == SDLK_LEFT)
                    leftPressed = false;
                if (event.key.keysym.sym == SDLK_RIGHT)
                    rightPressed = false;
                if(!leftPressed && !rightPressed)
                    dasDelayTick = ULONG_MAX;
            }
        }

        // Downward movement 
        bool moveTetro = false;
        if (!downPressed && 
                SDL_GetTicks() - vMoveTick >= NORMAL_SPEED)
            moveTetro = true;
        else if (downPressed && 
                SDL_GetTicks() - vMoveTick >= FAST_SPEED)
            moveTetro = true;

        if (moveTetro) {
            moveTetromino(t, 0, 1);
            vMoveTick = SDL_GetTicks();
        }

        // Horizontal movement
        if ((leftPressed && rightPressed) 
                || (!leftPressed && !rightPressed))
            xMove = 0;
        else if (leftPressed)
            xMove = -1;
        else if (rightPressed)
            xMove = +1;

        if (SDL_GetTicks() - dasDelayTick >= DAS_DELAY) {
            if (SDL_GetTicks() - hMoveTick >= HORIZONTAL_SPEED) {
                if (xMove != 0) {
                    moveTetromino(t, xMove, 0);
                    hMoveTick = SDL_GetTicks();
                }
            }
        }

        checkForFilledRow();

        SDL_RenderClear(renderer);
        renderWorld(renderer, tileTexture);
        renderTetromino(t, renderer, tileTexture);
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
