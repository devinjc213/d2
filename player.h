#ifndef PLAYER_H
#define PLAYER_H

#include "./shared/sprite.h"
#include "input.h"
#include <SDL2/SDL.h>

typedef struct {
    const char* name;
    int x, y;
    int health;
    AnimatedSprite* sprite;
} Player;

Player* create_player(int x, int y, const char* name);
void destroy_player(Player* player);
void update_player(Player* player, InputState* input, float delta_time);
void render_player(SDL_Renderer* renderer, Player* player);

#endif

