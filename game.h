#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "enemy.h"
#include <SDL2/SDL.h>

typedef struct {
    Player* player;
    Enemy** enemies;
    int num_enemies;
} Game;

Game* create_game();

void update_game(Game* game, InputState* input, float delta_time);
void render_game(SDL_Renderer* renderer, Game* game);
void destroy_game(Game* game);

#endif
