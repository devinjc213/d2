#ifndef ENEMY_H
#define ENEMY_H

#include "sprite.h"

typedef struct {
    int x, y;
    int health;
    AnimatedSprite* sprite;
} Enemy;

Enemy* create_enemy(int x, int y, int health, const char* sprite_name);
void destroy_enemy(Enemy* enemy);
void update_enemy(Enemy* enemy, float delta_time);
void render_enemy(SDL_Renderer* renderer, Enemy* enemy);

#endif
