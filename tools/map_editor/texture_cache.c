#include <SDL2/SDL.h>
#include "tilesheet.h"
#include "texture_cache.h"
#include "../../shared/logger.h"

void init_texture_cache(TextureCache* cache) {
    for (int i = 0; i < MAX_CACHE; i++) {
        cache->textures[i] = NULL;
        cache->loaded[i] = 0;
    }
}

SDL_Texture* load_texture(TextureCache* cache, SDL_Renderer* renderer, tilesheet_t tilesheet) {
    SDL_Texture* texture = load_tilesheet_only(renderer, tilesheet);

    cache->textures[tilesheet] = texture;
    cache->loaded[tilesheet] = 1;

    return texture;
}

SDL_Texture* get_texture(TextureCache* cache, SDL_Renderer* renderer, tilesheet_t tilesheet) {
    if (cache->loaded[tilesheet] == 1) {
        GINFO("Cache hit");
        return cache->textures[tilesheet];
    }

    GINFO("Cache miss");
    return load_texture(cache, renderer, tilesheet);
}

void destroy_texture_cache(TextureCache* cache) {
    for (int i = 0; i < MAX_CACHE; i++) {
        if (cache->loaded[i] == 1) {
            SDL_DestroyTexture(cache->textures[i]);
            cache->textures[i] = NULL;
            cache->loaded[i] = 0;
        }
    }

}
