#include "player.h"
#include "input.h"
#include <stdlib.h>

Player* create_player(int x, int y, const char* name) {
    Player* player = (Player*)malloc(sizeof(Player));
    if (player != NULL) {
        player->name = name;
        player->x = x;
        player->y = y;
        player->health = 10;
        player->sprite = find_animation("knight_m_idle_anim");
    }

    return player;
}

void destroy_player(Player* player) {
    free(player);
}

void update_player(Player* player, InputState* input, float delta_time) {
    if (input->up) player->y -= 5;
    if (input->down) player->y += 5;
    if (input->left) player->x -= 5;
    if (input->right) player->x += 5;

    update_animation(player->sprite, delta_time);
}

void render_player(SDL_Renderer* renderer, Player* player) {
    if (player->sprite != NULL) {
        render_animation(renderer, player->sprite, player->x, player->y);
    }
}
