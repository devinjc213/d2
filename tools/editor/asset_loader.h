#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H
#include <SDL2/SDL.h>

SDL_Texture* load_asset_to_window(SDL_Renderer* renderer, char *filename, int* width, int* height);
SDL_Texture* load_asset(SDL_Renderer* renderer, char *filename);
void free_asset(SDL_Texture* tilesheet);

#endif
