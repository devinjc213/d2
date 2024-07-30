#include "game.h"
#include "player.h"
#include "enemy.h"
#include "input.h"
#include <stdlib.h>

Game* create_game() {
    Game* game = (Game*)malloc(sizeof(Game));
    if (game != NULL) {
        game->player = create_player(100, 100, "Bob");
        game->num_enemies = 1;
        game->enemies = (Enemy**)malloc(sizeof(Enemy*) * game->num_enemies);
        for (int i = 0; i < game->num_enemies; i++) {
            //game->enemies[i] = create_enemy(50*i, 50);
        }
    }

    return game;
}

void update_game(Game* game, InputState* input, float delta_time) {
    update_player(game->player, input, delta_time);
    for (int i = 0; i < game->num_enemies; i++) {
        //update_enemy(game->enemies[i]);
    }
}

void render_game(SDL_Renderer* renderer, Game* game) {
    SDL_RenderClear(renderer);
    render_player(renderer, game->player);
    for (int i = 0; i < game->num_enemies; i++) {
        //render_enemy(game->enemies[i]);
    }
    SDL_RenderPresent(renderer);
}

void destroy_game(Game* game) {
    destroy_player(game->player);
    for (int i = 0; i < game->num_enemies; i++) {
        //destroy_enemy(game->enemies[i]);
    }
}
