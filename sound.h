
typedef enum
{
    MOVEMENT,
    ROTATION,
    HARD_DROP,
    LINE_CLEAR,
    PAUSE,
    GAME_OVER
    
} SoundEffect;

void initSound();
void playMusic();
void playSound(SoundEffect sound);
