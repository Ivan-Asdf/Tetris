#include "SDL2/SDL.h"

#define TILE_SIZE 32
#define GAME_WIDTH 10
#define GAME_HEIGHT 20

extern int grid[GAME_HEIGHT][GAME_WIDTH];

void renderWorld(SDL_Renderer* r, SDL_Texture* t);
void checkForFilledRow();


