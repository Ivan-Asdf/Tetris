#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "world.h"
#include "tetromino.h"
#include "sound.h"

#define FPS 60
#define NORMAL_SPEED 800
#define FAST_SPEED 50
#define HORIZONTAL_SPEED 70
#define DAS_DELAY 250

#define WINDOW_WIDTH GAME_WIDTH*TILE_SIZE
#define WINDOW_HEIGHT GAME_HEIGHT*TILE_SIZE

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* tetrominoTileTexture = NULL;
SDL_Texture* ghostTileTexture = NULL;
SDL_Texture* lockedTileTexture = NULL;

bool debugMode = false;

void initGame();
int exitGame();
void capFramerate(Uint32 startTick);

int main(int argc, char* argv[])
{
    initGame();
    Tetromino* t = spawnTetromino();

    Uint32 startFrameTick;
    Uint32 yMoveTick = SDL_GetTicks();
    Uint32 xMoveTick = SDL_GetTicks();
    Uint32 dasDelayTick = ULONG_MAX;

    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;

    int xMove = 0;
    int yMove = 0;
    int rotation = 0;

    while (1) {
        startFrameTick = SDL_GetTicks();

        // Event handling
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                return exitGame();
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.repeat == 0) {
                    switch(event.key.keysym.sym) {
                        case SDLK_UP:
                            if(debugMode)
                                moveTetromino(&t, 0, -1);
                            break;
                        case SDLK_DOWN:
                            if(debugMode)
                                moveTetromino(&t, 0, 1);
                            downPressed = true;
                            break;
                        case SDLK_LEFT:
                            leftPressed = true;
                            moveTetromino(&t, -1, 0);
                            break;
                        case SDLK_RIGHT:
                            rightPressed = true;
                            moveTetromino(&t, 1, 0);
                            break;
                        case SDLK_x:
                            rotation = 1;
                            break;
                        case SDLK_z:
                            rotation = -1;
                            break;
                        case SDLK_SPACE:
                            hardDropTetromino(&t);
                            break;
                        case SDLK_F5:
                            debugMode = !debugMode;
                            printf("Debug mode: %d\n", debugMode);
                            playSound(PAUSE);
                    }

                    if(leftPressed || rightPressed)
                        dasDelayTick = SDL_GetTicks();
                }
            }
            else if (event.type == SDL_KEYUP) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        upPressed = false;
                        break;
                    case SDLK_DOWN:
                        downPressed = false;
                        break;
                    case SDLK_LEFT:
                        leftPressed = false;
                        break;
                    case SDLK_RIGHT:
                        rightPressed = false;
                        break;
                }
                if(!leftPressed && !rightPressed)
                    dasDelayTick = ULONG_MAX;
            }
        }

        // Downward movement 
        yMove = 0;
        if (!downPressed && SDL_GetTicks() - yMoveTick >= NORMAL_SPEED)
            yMove = 1;
        else if (downPressed && SDL_GetTicks() - yMoveTick >= FAST_SPEED)
            yMove = 1;

        if (debugMode)
            yMove = 0;

        if (yMove) {
            moveTetromino(&t, 0, yMove);
            yMoveTick = SDL_GetTicks();
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
            if (SDL_GetTicks() - xMoveTick >= HORIZONTAL_SPEED) {
                moveTetromino(&t, xMove, 0);
                xMoveTick = SDL_GetTicks();
            }
        }
        // Rotation
        rotateTetromino(t, rotation);
        rotation = 0;

        checkForFilledRow();

        SDL_RenderClear(renderer);
        renderWorld(renderer, lockedTileTexture);
        renderTetromino(t, renderer, tetrominoTileTexture);
        Tetromino ghostT = getGhostTetromino(t);
        renderTetromino(&ghostT, renderer, ghostTileTexture);
        SDL_RenderPresent(renderer);

        capFramerate(startFrameTick);
    }
}

void initGame()
{
    initSound();
    playMusic();
    srand(time(0));
    SDL_Init(SDL_INIT_EVERYTHING); 

    window = SDL_CreateWindow("MUh Title",
            800, 400,
            WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    tetrominoTileTexture = IMG_LoadTexture(renderer, "gfx/tile.png");
    ghostTileTexture = IMG_LoadTexture(renderer, "gfx/ghost_tile.png");
    lockedTileTexture = IMG_LoadTexture(renderer, "gfx/locked_tile.png");
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
