#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include "defs.h"
#include "logger.h"
#include "sprite.h"
#include "game.h"
#include "input.h"

int init();
void shutdown_game();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

int main() {
    if (init() > 0) {
        GFATAL("Failed to initialize\n");
    }

    if (init_sprites(gRenderer, "assets/tileset.png", "tile_coords") > 0) {
        GFATAL("Failed to initialize sprites");
    }

    float delta_time = 1.0f / 144.0f;

    Game* game = create_game();
    InputState input;
    init_input(&input);

    int quit = 0;
    while (quit != 1) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else {
                handle_input(&e, &input);
            }
        }

        update_game(game, &input, delta_time);
        render_game(gRenderer, game);
    }

    destroy_game(game);
    shutdown_game();

    return 0;
}

int init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        GFATAL("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    } 

    gWindow = SDL_CreateWindow(
        "SDL2 Window",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (gWindow == NULL) {
        GFATAL("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    gRenderer = SDL_CreateRenderer(gWindow,
                                   -1,
                                   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == NULL) {
        GFATAL("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    if (!IMG_Init(IMG_INIT_PNG)) {
        GFATAL("SDL_image could not initialize! SDL_Error: %s\n", IMG_GetError());
        return 1;
    }

    return 0;
}

void shutdown_game() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    gWindow = NULL;
    gRenderer = NULL;

    IMG_Quit();
    SDL_Quit();
}

