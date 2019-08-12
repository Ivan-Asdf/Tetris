#include "tetromino.h"

typedef struct {
    int x;
    int y;
} Offset;

Offset kickData[8][5];
Offset kickDataI[8][5];

Offset* getWallKickOffsets(int from, int to, TetrominoType type) {
    int indexBase = from * 2; // formed by from
    int indexAdd;
    if(to == 4 && from == 0)
        indexAdd = 0;
    else if(to > from)
        indexAdd = 1;
    else if(to < from)
        indexAdd = 0;

    int index = indexBase + indexAdd;

    if(type == I)
        return kickDataI[index];

    return kickData[index];
}

// https://tetris.wiki/SRS
Offset kickData[8][5] ={
    {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},      // 0 -> 4
    {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},   // 0 -> 1
    {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -1}},     // 1 -> 0
    {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},     // 1 -> 2
    {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},   // 2 -> 1
    {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},      // 2 -> 3 
    {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},  // 3 -> 2
    {{0, 0}, {-1, 0}, {-1, -1}, {0, -2}, {-1, -2}}  // 3 -> 0
};

Offset kickDataI[8][5] ={
    {{0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1}},    // 0 -> 4
    {{0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2}},    // 0 -> 1
    {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},    // 1 -> 0
    {{0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, -1}},   // 1 -> 2
    {{0, 0}, {1, 0}, {2, 0}, {1, 2}, {-2, 1}},      // 2 -> 1
    {{0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2}},    // 2 -> 3 
    {{0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2}},    // 3 -> 2
    {{0, 0}, {1, 0}, {2, 0}, {1, -2}, {-2, 1}}      // 3 -> 0
};
