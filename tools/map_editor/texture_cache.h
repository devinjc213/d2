#ifndef TEXTURE_CACHE_H 
#define TEXTURE_CACHE_H 
#include <SDL2/SDL.h>
#include "tilesheet.h"

#define MAX_CACHE 31

typedef struct {
  SDL_Texture* textures[MAX_CACHE];
  int loaded[MAX_CACHE];
} TextureCache;

void init_texture_cache(TextureCache* cache);
SDL_Texture* load_texture(TextureCache* cache, SDL_Renderer* renderer, tilesheet_t tilesheet);
SDL_Texture* get_texture(TextureCache* cache, SDL_Renderer* renderer, tilesheet_t tilesheet);
void destroy_texture_cache(TextureCache* cache);

#endif
