#include <stdbool.h>

#include "world.h"


void renderWorld(SDL_Renderer* r, SDL_Texture* t) {
    for (int y = 0; y < GAME_HEIGHT; ++y) {
        for (int x = 0; x < GAME_WIDTH; ++x) {
            SDL_Rect textureRect = {32*grid[y][x], 0, 32, 32};
            SDL_Rect placementRect = {32*x, 32*y, 32, 32};
            SDL_RenderCopy(r, t, &textureRect, &placementRect);
        }
    }
}

void checkForFilledRow() {
    for (int y = 0; y < GAME_HEIGHT; ++y) {
        bool rowFilled = true;
        for (int x = 0; x < GAME_WIDTH; ++x) {
            if (grid[y][x] == 0)
                rowFilled = false;
        }
        if (rowFilled) {
            for (int y1 = y - 1; y1 >=0 ; --y1) {
                for (int x = 0; x < GAME_WIDTH; ++x)
                    grid[y1 + 1][x] = grid[y1][x];
            }
        }
    }
}
