#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include "defs.h"
#include "logger.h"
#include "sprite.h"

int init();
void closeGame();
SDL_Texture* loadTexture(const char* path);

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTileSheet = NULL;

SpriteMap sprite_map;
AnimatedSpriteMap animation_map;
SDL_Texture* spritesheet = NULL;

int main() {
    if (init() > 0) {
        GFATAL("Failed to initialize\n");
    }

    if (init_sprites(
            gRenderer,
            &spritesheet,
            &sprite_map,
            "images/tileset.png",
            "tile_coords"
        ) > 0) {
        GFATAL("Failed to initialize sprites");
    }

    load_animations(&sprite_map, &animation_map);

    float delta_time = 1.0f / 144.0f;

    int quit = 0;
    while (quit != 1) {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        for (int i = 0; i < animation_map.count; i++) {
            update_animation(&animation_map.sprites[i], delta_time);
        }

        SDL_RenderClear(gRenderer);

        render_sprite(gRenderer,
                      &sprite_map,
                      spritesheet,
                      "big_zombie_run_anim_f2",
                      100,
                      100);
        render_sprite(gRenderer,
                      &sprite_map,
                      spritesheet,
                      "chest_full_open_anim_f0",
                      200,
                      200);

        render_animation(gRenderer,
                         &sprite_map,
                         spritesheet,
                         find_animation(&animation_map, "big_zombie_run_anim"),
                         300,
                         300);

        SDL_RenderPresent(gRenderer);
    }

    closeGame();

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

SDL_Texture* loadTexture(const char* path) {
    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        GERROR("Unable to load image %s! SDL_image Error: %s\n",
               path,
               IMG_GetError());
    }

    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (newTexture == NULL) {
        GERROR("Unable to create texture from %s! SDL Error: %s\n",
               path,
               SDL_GetError());
    }

    return newTexture;
}

void closeGame() {
    SDL_DestroyTexture(gTileSheet);
    gTileSheet = NULL;

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    IMG_Quit();
    SDL_Quit();
}

