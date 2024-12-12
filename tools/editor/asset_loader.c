#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../../shared/logger.h"
#include "asset_loader.h"

SDL_Texture* load_asset_to_window(SDL_Renderer* renderer, char *file_path, int* width, int* height) {
    SDL_Surface* surface = IMG_Load(file_path);
    if (surface == NULL) {
        GFATAL("Failed to load spritesheet: %s\n", IMG_GetError());
        return NULL;
    }

    *width = surface->w;
    *height = surface->h;

    SDL_Texture* tilesheet = SDL_CreateTextureFromSurface(renderer, surface);
    if(tilesheet == NULL) {
        GFATAL("Failed to create spritesheet texture: %s\n", SDL_GetError());
        return NULL;
    }

    return tilesheet;
}

SDL_Texture* load_asset(SDL_Renderer* renderer, char *file_path) {
    GINFO("Attempting to load asset: %s\n");
    SDL_Surface* surface = IMG_Load(file_path);
    if (surface == NULL) {
        GFATAL("Failed to load spritesheet: %s\n", IMG_GetError());
        return NULL;
    }

    SDL_Texture* tilesheet = SDL_CreateTextureFromSurface(renderer, surface);
    if(tilesheet == NULL) {
        GFATAL("Failed to create spritesheet texture: %s\n", SDL_GetError());
        return NULL;
    }

    return tilesheet;
}

void free_asset(SDL_Texture* tilesheet) {
    free(tilesheet);
}
