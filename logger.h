#pragma once

#include "tetromino.h"

void logSpawnSet(const Tetromino* const tSpawnSet);
void logTetromino(Tetromino* t, const char* prefix);

static char getTetrominoTypeLetter(const Tetromino* const t);
