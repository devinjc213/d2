#ifndef TEXTURE_CACHE_H 
#define TEXTURE_CACHE_H 
#include <SDL2/SDL.h>
#include "defs.h"

typedef struct {
  char *name;
  SDL_Texture *texture;
} Texture;

typedef struct {
  Texture* textures[MAX_ASSET_FILES];
  int count;
} TextureCache;

void init_texture_cache(TextureCache* cache);
SDL_Texture* load_texture(TextureCache* cache, SDL_Renderer* renderer, char *file_path);
SDL_Texture* get_texture(TextureCache* cache, SDL_Renderer* renderer, char *file_path);
void destroy_texture_cache(TextureCache* cache);

#endif
