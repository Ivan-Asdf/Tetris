#pragma once

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

void moveTetromino(Tetromino** pt, int x, int y);
static void moveTetrominoNoColl(Tetromino* t, int xOffset, int yOffset);
void rotateTetromino(Tetromino* t, bool clockwise);
bool checkCollision(Tetromino* t);

void renderTetromino(Tetromino* t, SDL_Renderer* r, SDL_Texture* texture);
