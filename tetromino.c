#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "world.h"
#include "wallkick.h"
#include "logger.h"
#include "sound.h"

#include "tetromino.h"

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

Tetromino* tSpawnSet = NULL;
int currentT = -1;

Tetromino* spawnTetromino()
{
    if(currentT < 7 && currentT >= 0) {
        return &tSpawnSet[currentT++];
    }

    currentT = 0;
    generateTetrominoSpawnSet();
    return spawnTetromino();
}

Tetromino getGhostTetromino(const Tetromino* const t)
{
    Tetromino ghost = *t;

    while (!checkCollision(&ghost))
        moveTetrominoNoColl(&ghost, 0, 1);

    moveTetrominoNoColl(&ghost, 0, -1);
    return ghost;
}

void moveTetromino(Tetromino** pt, int x, int y)
{
    Tetromino temp = **pt;
    moveTetrominoNoColl(*pt, x, y);

    if(checkCollision(*pt)) {
        **pt = temp;
        // Hit locked tiles
        if (y > 0) {
            printf("%d\n", y);
            lockTetromino(pt);
            return;
        }
    }

    if(x || y) {
        logTetromino(*pt, "Moved: ");
        playSound(MOVEMENT);
    }
}

void hardDropTetromino(Tetromino** pt)
{
    **pt = getGhostTetromino(*pt);
    lockTetromino(pt);
    playSound(HARD_DROP);
}

// Rotation: +1 (clockwise), -1(counterclockwise)
void rotateTetromino(Tetromino* t, int rotation)
{
    if (rotation == 0 || (rotation != 1 && rotation != -1))
        return;
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
                shiftV = rotation;
            else
                shiftV = -rotation;
        }
        // If horizontal 
        else {
            if (t->tiles[0].x < t->tiles[1].x)
                shiftH = rotation; 
            else
                shiftH = -rotation;
        }

        for(int i = 0; i < TILES_PER_TETROMINO; ++i) {
            t->tiles[i].x += shiftH;
            t->tiles[i].y += shiftV;
        }
    }

    // Rotation
    int centerTileIndex = 0;
    if(t->type == I && rotation == -1)
        centerTileIndex = 1;
    Tile centerTile = t->tiles[centerTileIndex];

    for (int i = 0; i < TILES_PER_TETROMINO; ++i) {
        if(i == centerTileIndex) continue;

        Tile* outerTile = &t->tiles[i]; 
        int deltaX, deltaY;
        deltaX = (centerTile.x - outerTile->x) * rotation;
        deltaY = (centerTile.y - outerTile->y) * rotation;

        outerTile->y = centerTile.y - deltaX;
        outerTile->x = centerTile.x + deltaY;
    }

    // Wall kicks
    int from = t->rotationState;
    int to = from + rotation;
    if (to > 3) 
        to = 0;
    else if (to < 0)
        to = 3;

    Offset* offset = getWallKickOffsets(from, to, t->type);
    printf("Rotation: %d -> %d  Wallkick offsets tried:", from, to);
    Tetromino rotatedOrigTetromino = *t;
    for (int i = 0; i < 5; ++i) {
        printf(" (%d, %d)", offset[i].x, offset[i].y);
        moveTetrominoNoColl(t, offset[i].x, offset[i].y);
        if (checkCollision(t) == true) {
            *t = rotatedOrigTetromino;
        }
        else {
            t->rotationState = to;
            logTetromino(t, "  Final position: ");
            playSound(ROTATION);
            return;
        }
    }

    printf(" Rotation failed\n");
    *t = temp;
}

void renderTetromino(Tetromino* t, SDL_Renderer* r, SDL_Texture* texture)
{
    for (int i = 0; i < TILES_PER_TETROMINO; ++i)
    {
        int tileX = t->tiles[i].x;
        int tileY = t->tiles[i].y;
        SDL_Rect textureRect = {32*t->type, 0, 32, 32};
        SDL_Rect placementRect = {32*tileX, 32*tileY, 32, 32};
        SDL_RenderCopy(r, texture, &textureRect, &placementRect);
    }
}

static bool checkCollision(Tetromino* t) { 
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

static void moveTetrominoNoColl(Tetromino* t, int x, int y) {
    for(int i = 0; i < TILES_PER_TETROMINO; ++i)
    {
        t->tiles[i].x += x;
        t->tiles[i].y += y;
    }
}

static void lockTetromino(Tetromino** pt)
{       
    // Lock tetromino tiles
    for (int i = 0; i < TILES_PER_TETROMINO; ++i) { 
        int tileX = (*pt)->tiles[i].x;
        int tileY = (*pt)->tiles[i].y;
        if (tileY < 0) {
            printf("GameOver\n");
            playSound(GAME_OVER);
            exit(0);
        }
        grid[(*pt)->tiles[i].y][(*pt)->tiles[i].x] = (*pt)->type+1;
    }
    logTetromino(*pt, "Locked: ");
    // Spawn new tetromino
    *pt = spawnTetromino();
}

static void generateTetrominoSpawnSet()
{
    if(tSpawnSet == NULL)
        tSpawnSet = (Tetromino*)malloc(sizeof(Tetromino)*NUMBER_OF_TETROMINOES);

    int set[NUMBER_OF_TETROMINOES] = {-1, -1, -1, -1, -1, -1, -1};
    for (int i = 0; i < NUMBER_OF_TETROMINOES;)
    {
        int number = rand() % 7;        
        bool unique = true;
        for (int j = 0; j < NUMBER_OF_TETROMINOES; ++j)
            if(number == set[j])
                unique = false;
        if(unique) {
            set[i] = number;
            ++i;
        }
    }

    for (int i = 0; i < NUMBER_OF_TETROMINOES; ++i) {
        Tetromino t;

        t.rotationState = 0;
        t.type  = set[i];
        //printf("Spawn: ");
        for (int j = 0; j < TILES_PER_TETROMINO; ++j) 
        {
            t.tiles[j] = spawn[t.type][j];
            t.tiles[j].x += 3;
            t.tiles[j].y -= 2;
            //printf("(%d %d)", t.tiles[j].x, t.tiles[j].y);
        }
        //putchar('\n');

        tSpawnSet[i] = t;
    }
    logSpawnSet(tSpawnSet);
}
