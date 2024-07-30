#ifndef ENEMY_H
#define ENEMY_H

#include "sprite.h"

typedef struct {
    int x, y;
    int health;
    AnimatedSprite* spirte;
} Enemy;

Enemy* create_enemy(int x, int y);
void destroy_enemy(Enemy* enemy);
void update_enemy(Enemy* enemy);
void render_enemy(Enemy* enemy);

#endif
