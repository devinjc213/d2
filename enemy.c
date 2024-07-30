#include "enemy.h"
#include <stdlib.h>

Enemy* create_enemy(int x, int y, int health, const char* sprite_name) {
    Enemy* enemy = (Enemy*)malloc(sizeof(Enemy));
    if (enemy != NULL) {
        enemy->health = health;
        enemy->x = x;
        enemy->y = y;
        enemy->sprite = find_animation(sprite_name);
    }

    return enemy;
}

void destroy_enemy(Enemy* enemy) {
    free(enemy);
}

void update_enemy(Enemy* enemy, float delta_time) {
    update_animation(enemy->sprite, delta_time);
}

void render_enemy(SDL_Renderer* renderer, Enemy* enemy) {
    if (enemy->sprite != NULL) {
        render_animation(renderer, enemy->sprite, enemy->x, enemy->y);
    }
}
