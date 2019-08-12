
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
} TetronimoType;

typedef struct {
    Tile tiles[4];
    TetronimoType  type;
} Tetromino;

Tetromino* spawnTetromino();

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

typedef enum {
    WALL_LEFT = 0,
    WALL_RIGHT,
    WALL_DOWN,
    STATIC_LEFT = 10,
    STATIC_RIGHT,
    STATIC_DOWN,
    STATIC_UP,
    STATIC_OMNI

} CollisionDirection;

typedef struct {
    bool isCollision;
    Direction d;
    int depth;
} CollInfo;


bool moveTetromino(Tetromino* t, Direction d, int distance);
void rotateTetromino(Tetromino* t);
void checkAndResolveCollision(Tetromino* t);
CollInfo checkCollision(Tetromino* ot, Tetromino* t);
void renderTetromino(Tetromino* t, SDL_Renderer* r, SDL_Texture* texture);
