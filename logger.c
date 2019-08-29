#include <stdio.h>

#include "logger.h"

void logSpawnSet(const Tetromino* const tSpawnSet)
{
    printf("Generated spawn set: ");
    for (int i = 0; i < NUMBER_OF_TETROMINOES; ++i) {
        printf("%c ", getTetrominoTypeLetter(tSpawnSet + i));
    }
    putchar('\n');
}

void logTetromino(Tetromino* t, const char* prefix)
{
    printf("%s", prefix);
    for (int i = 0; i < TILES_PER_TETROMINO; ++i)
        printf("(%d %d) ", t->tiles[i].x, t->tiles[i].y);
    putchar(getTetrominoTypeLetter(t));
    putchar('\n');
}

static char getTetrominoTypeLetter(const Tetromino* const t)
{
    switch (t->type) {
    case I:
        return 'I';
    case T:
        return 'T';
    case J:
        return 'J';
    case L:
        return 'L';
    case Z:
        return 'Z';
    case S:
        return 'S';
    case O:
        return 'O';
    }
}
