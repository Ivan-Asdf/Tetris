#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "world.h"
#include "wallkick.h"

#include "tetromino.h"

#define NUMBER_OF_TETROMINOES 7
#define TILES_PER_TETROMINO 4

Tile spawn[NUMBER_OF_TETROMINOES][TILES_PER_TETROMINO] = 
{
    // First member is the center of location (First two for "I")
    {{1, 1}, {2, 1}, {0, 1}, {3, 1}},   // I 
    {{1, 1}, {0, 1}, {1, 0}, {2, 1}},   // T 
    {{1, 1}, {0, 1}, {0, 0}, {2, 1}},   // J
    {{1, 1}, {0, 1}, {2, 1}, {2, 0}},   // L
    {{1, 1}, {0, 1}, {1, 0}, {2, 0}},   // Z
    {{1, 1}, {1, 0}, {0, 0}, {2, 1}},   // S
    {{1, 1}, {2, 0}, {1, 0}, {2, 1}},   // O
};


Tetromino* spawnTetromino()
{
    Tetromino* t = (Tetromino*)malloc(sizeof(Tetromino));

    t->rotationState = 0;
    t->type = rand() % 7;
    printf("Spawn: ");
    for (int i = 0; i < 4; ++i) 
    {
        t->tiles[i] = spawn[t->type][i];
        t->tiles[i].x += 3;
        t->tiles[i].y -= 2;
        printf("(%d %d)", t->tiles[i].x, t->tiles[i].y);
    }
    putchar('\n');

    return t;
}

void moveTetromino(Tetromino** pt, int x, int y)
{
    Tetromino temp = **pt;
    moveTetrominoNoColl(*pt, x, y);

    // Hit locked tiles
    if(y > 0 && checkCollision(*pt)) {
        // Lock tetromino tiles
        for (int i = 0; i < TILES_PER_TETROMINO; ++i) { 
            int tileX = (*pt)->tiles[i].x;
            int tileY = (*pt)->tiles[i].y;
            printf("Lock tile: (%d %d)\n", tileX, tileY);
            if (tileY < 0) {
                printf("GameOver\n");
                exit(0);
            }
            grid[temp.tiles[i].y][temp.tiles[i].x] = (*pt)->type+1;
        }
        // Spawn new tetromino
        free(*pt);
        *pt = spawnTetromino();
    }
    else if(checkCollision(*pt))
        **pt = temp;
}

static void moveTetrominoNoColl(Tetromino* t, int xOffset, int yOffset) {
    for(int i = 0; i < TILES_PER_TETROMINO; ++i)
    {
        t->tiles[i].x += xOffset;
        t->tiles[i].y += yOffset;
    }
}

void rotateTetromino(Tetromino* t, bool clockwise)
{
    if (t->type == O)
        return;

    Tetromino temp = *t;

    // Special ration case for "I" 
    if (t->type == I) {
        int shiftH = 0;
        int shiftV = 0;

        // If vertical
        if (t->tiles[0].x == t->tiles[1].x) {
            if (t->tiles[0].y < t->tiles[1].y)
                shiftV = 1;
            else
                shiftV = -1;
        }
        // If horizontal 
        else {
            if (t->tiles[0].x < t->tiles[1].x)
                shiftH = 1; 
            else
                shiftH = -1;
        }

        if (!clockwise) {
            shiftH = -shiftH;
            shiftV = -shiftV;
        }

        for(int i = 0; i < TILES_PER_TETROMINO; ++i) {
            t->tiles[i].x += shiftH;
            t->tiles[i].y += shiftV;
        }
    }

    // Rotation
    int centerTileIndex = 0;
    if(t->type == I && !clockwise)
        centerTileIndex = 1;
    Tile centerTile = t->tiles[centerTileIndex];

    for (int i = 0; i < TILES_PER_TETROMINO; ++i) {
        if(i == centerTileIndex) continue;

        Tile* outerTile = &t->tiles[i]; 
        int deltaX, deltaY;
        deltaX = centerTile.x - outerTile->x;
        deltaY = centerTile.y - outerTile->y;

        if(!clockwise) {
            deltaX = -deltaX;
            deltaY = -deltaY;
        }

        outerTile->y = centerTile.y - deltaX;
        outerTile->x = centerTile.x + deltaY;
    }

    // Wall kicks
    int from = t->rotationState;
    int to;
    if (clockwise)
        to = (from + 1) % 4;
    else
        to = (from != 0 ? from - 1 : 3);

    Offset* offset = getWallKickOffsets(from, to, t->type);
    printf("Rotation: %d -> %d\n", from, to);
    Tetromino rotatedOrigTetromino = *t;
    for (int i = 0; i < 5; ++i) {
        printf("(%d, %d)\n", offset[i].x, offset[i].y);
        moveTetrominoNoColl(t, offset[i].x, offset[i].y);
        if (checkCollision(t) == true) {
            *t = rotatedOrigTetromino;
        }
        else {
            t->rotationState = to;
            return;
        }
    }

    *t = temp;
}

bool checkCollision(Tetromino* t) {
    for (int i = 0; i < TILES_PER_TETROMINO; ++i) {
        int tileX = t->tiles[i].x;
        int tileY = t->tiles[i].y;
        if (tileX < 0)
            return true;
        else if (tileX > GAME_WIDTH - 1)
            return true;
        else if (tileY > GAME_HEIGHT - 1)
            return true;

        if (tileX >= 0 && tileY >= 0)
            if (grid[tileY][tileX] != 0)
                return true;
    }
    return false;
}


void renderTetromino(Tetromino* t, SDL_Renderer* r, SDL_Texture* texture)
{
    for (int i = 0; i < TILES_PER_TETROMINO; ++i)
    {
        int tileX = t->tiles[i].x;
        int tileY = t->tiles[i].y;
        SDL_Rect textureRect = {32*(t->type + 1), 0, 32, 32};
        SDL_Rect placementRect = {32*tileX, 32*tileY, 32, 32};
        SDL_RenderCopy(r, texture, &textureRect, &placementRect);
    }
}

