#pragma once

#include <stdbool.h>

#include "SDL2/SDL.h"

#define NUMBER_OF_TETROMINOES 7
#define TILES_PER_TETROMINO 4

typedef struct
{
    int x,y;
} Tile;

typedef enum
{
    I,
    T,
    J,
    L,
    Z,
    S,
    O
} TetrominoType;

typedef struct {
    Tile tiles[4];
    TetrominoType  type;
    int rotationState;
} Tetromino;

Tetromino* spawnTetromino();
Tetromino getGhostTetromino(const Tetromino* const t);

// Logic
void moveTetromino(Tetromino** pt, int x, int y);
void hardDropTetromino(Tetromino** pt);
void rotateTetromino(Tetromino* t, int rotation);
// Render
void renderTetromino(Tetromino* t, SDL_Renderer* r, SDL_Texture* texture);

static bool checkCollision(Tetromino* t);
static void moveTetrominoNoColl(Tetromino* t, int x, int y);
static void lockTetromino(Tetromino** pt);
static void generateTetrominoSpawnSet();

