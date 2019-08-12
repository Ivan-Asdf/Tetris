#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "world.h"

#include "tetromino.h"

#define NUMBER_OF_TETROMINOES 7
#define TILES_PER_TETROMINO 4

Tile spawn[NUMBER_OF_TETROMINOES][TILES_PER_TETROMINO] = 
{
    // First member is the center of location (First two for "I")
    {{1, 0}, {2, 0}, {0, 0}, {3, 0}},   // I (spawns sideways)
    {{1, 1}, {0, 1}, {1, 2}, {2, 1}},   // T (spans upside down)
    {{1, 1}, {0, 1}, {0, 0}, {2, 1}},   // J
    {{1, 1}, {0, 1}, {2, 1}, {2, 0}},   // L
    {{1, 1}, {0, 1}, {1, 0}, {2, 0}},   // Z
    {{1, 1}, {1, 0}, {0, 0}, {2, 1}},   // S
    {{1, 1}, {2, 0}, {1, 0}, {2, 1}},   // O
};

Tetromino* spawnTetromino()
{
    Tetromino* t = (Tetromino*)malloc(sizeof(Tetromino));

    t->type = rand() % 7;
    t->type = I;
    for (int i = 0; i < 4; ++i) 
    {
        t->tiles[i] = spawn[t->type][i];
        t->tiles[i].x += 3;
    }

    printf("Spawned Tetromino\n");
    return t;
}

bool moveTetromino(Tetromino* t, Direction d)
{
    Tetromino temp = *t;
    for(int i = 0; i < TILES_PER_TETROMINO; ++i)
    {
        if (d == DOWN)
            t->tiles[i].y++;
        else if(d == UP)
            t->tiles[i].y--;
        else if(d == LEFT)
            t->tiles[i].x--;
        else if(d == RIGHT)
            t->tiles[i].x++;
    }

    int coll = checkCollision(&temp, t);
    printf("%d\n", coll);
    if (coll != -1) {
        if (d == LEFT || d == RIGHT) {
            *t = temp;
            return false;
        }
        else if (d == DOWN) {
            for (int j = 0; j < TILES_PER_TETROMINO; ++j) {
                Tile tile = temp.tiles[j];
                grid[tile.y][tile.x] = temp.type + 1;
            }
            // Spawn new teromino
            free(t);
            t = spawnTetromino();
            return true;
        }
    }
    return true;

}

void rotateTetromino(Tetromino* t)
{
    if (t->type == O)
        return;

    Tetromino temp = *t;

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

        for(int i = 0; i < TILES_PER_TETROMINO; ++i) {
            t->tiles[i].x += shiftH;
            t->tiles[i].y += shiftV;
        }
    }

    // Rotate
    Tile centerTile = t->tiles[0];
    for (int i = 1; i < TILES_PER_TETROMINO; ++i) {
        Tile* outerTile = &t->tiles[i]; 
        int deltaX, deltaY;
        deltaX = centerTile.x - outerTile->x;
        deltaY = centerTile.y - outerTile->y;

        outerTile->y = centerTile.y - deltaX;
        outerTile->x = centerTile.x + deltaY;
    }

    auto coll = checkCollision(&temp, t);
    printf("Rotate collision: %d\n", coll);
    if (coll != -1) {
        Direction d;
        if(coll == STATIC_OMNI)
            *t = temp;
        else if(coll == STATIC_LEFT)
            d = RIGHT;
        else if(coll == STATIC_RIGHT)
            d = LEFT;

        if (moveTetromino(t, d) == false) {
            printf("Failed to move after roating");
            *t = temp;
        }
    }

    /*
    Direction collision = checkCollision(t);
    if (collision != -1) {
        while(collision != -1) {
            if (colision == LEFT) {
                moveTetromino(t, RIGHT);
            }
            esle if (colision == RIGHT) {
                moveTetromino(t, RIGHT);
            }
        }
    }
    */
    //checkAndResolveCollision(t);
}

/*
void checkAndResolveCollision(Tetromino* t) {
    for (int i = 0; i < TILES_PER_TETROMINO; ++i) {
        int tileX = t->tiles[i].x;
        int tileY = t->tiles[i].y;
        if (tileX < 0) {
            moveTetromino(t, RIGHT);
            return;
        }
        else if (tileX > GAME_WIDTH - 1) {
            moveTetromino(t, LEFT);
            return;
        }
        else if (tileY < 0) {
            moveTetromino(t, DOWN);
            return;
        }

        // Check if reach floor
        if (tileY > GAME_HEIGHT - 1 || grid[tileY][tileX] != 0) {
            moveTetromino(t, UP);
            for (int j = 0; j < TILES_PER_TETROMINO; ++j) {
                Tile tile = t->tiles[j];
                grid[tile.y][tile.x] = t->type + 1;
            }
            // Spawn new teromino
            free(t);
            t = spawnTetromino();
            return;
        }

    } 
}
*/

CollInfo checkCollision(Tetromino* ot, Tetromino* t) {
    bool left = false, right = false, up = false, down = false;
    for (int i = 0; i < TILES_PER_TETROMINO; ++i) {
        int tileX = t->tiles[i].x;
        int tileY = t->tiles[i].y;
        if (tileX < 0)
            left = true;
        else if (tileX > GAME_WIDTH - 1)
            right = true;
        else if (tileY > GAME_HEIGHT - 1)
            up = true;

        // Check if hit static tiles
        if (grid[tileY][tileX] != 0) {
            // Compare to all original tiles to determine direction.
            bool leftFromOrigTiles = true;
            bool rightFromOrigTiles = true;
            bool upFromOrigTiles = true;
            bool downFromOrigTiles = true;
            for (int j = 0; j < TILES_PER_TETROMINO; ++j) {
                int origTileX = ot->tiles[j].x;
                int origTileY = ot->tiles[j].y;

                if (!(tileX < origTileX))
                    leftFromOrigTiles = false;
                if (!(tileX > origTileX))
                    rightFromOrigTiles = false;
                if (!(tileY < origTileY))
                    upFromOrigTiles = false;
                if (!(tileY > origTileY))
                    downFromOrigTiles = false;
            }
            if (leftFromOrigTiles)
                left = true;
            if (rightFromOrigTiles)
                right = true;
            if (upFromOrigTiles)
                up = true;
            if (downFromOrigTiles)
                down = true;
        }
    } 

    if(left && right)
        return STATIC_OMNI;
    if(up && down)
        return STATIC_OMNI;
    if(left)
        return STATIC_LEFT;
    if(right)
        return STATIC_RIGHT;
    if(up)
        return STATIC_UP;
    if(down)
        return STATIC_DOWN;

    return -1;
}

void renderTetromino(Tetromino* t, SDL_Renderer* r, SDL_Texture* texture)
{
    for (int i = 0; i < 4; ++i)
    {
        SDL_Rect textureRect = {32*(i + 1), 0, 32, 32};
        SDL_Rect placementRect;
        placementRect.x = 32*(t->tiles[i].x);
        placementRect.y = 32*(t->tiles[i].y);
        placementRect.w = 32;
        placementRect.h = 32;
        SDL_RenderCopy(r, texture, &textureRect, &placementRect);
    }
}

