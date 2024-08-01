#define SCREEN_WIDTH 1920 
#define SCREEN_HEIGHT 1080 

#define TILE_SIZE 16

#include "../../shared/sprite.h"

typedef struct { 
    SDL_Rect rect;
    Sprite* sprite;
    int collidable;
    int layer;
} Block;
