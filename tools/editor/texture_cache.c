#include <SDL2/SDL.h>
#include "asset_loader.h"
#include "texture_cache.h"
#include "../../shared/logger.h"
#include "defs.h"

void
init_texture_cache(TextureCache* cache) {
    for (int i = 0; i < MAX_ASSET_FILES; i++) {
        cache->textures[i] = NULL;
    }
    cache->count = 0;
}

SDL_Texture*
load_texture(TextureCache* cache, SDL_Renderer* renderer, char *file_path) {
    GINFO("Attempting to load texture: %s\n", file_path);
    
    cache->textures[cache->count] = malloc(sizeof(Texture));
    if (cache->textures[cache->count] == NULL) {
        GERROR("Failed to allocate memory for texture\n");
        return NULL;
    }

    SDL_Texture* texture = load_asset(renderer, file_path);
    if (texture == NULL) {
        GERROR("Failed to load texture: %s\n", file_path);
        free(cache->textures[cache->count]);
        return NULL;
    }

    cache->textures[cache->count]->name = strdup(file_path);
    cache->textures[cache->count]->texture = texture;
    
    return cache->textures[cache->count++]->texture;
}

SDL_Texture*
get_texture(TextureCache* cache, SDL_Renderer* renderer, char *file_path) {
    for (int i = 0; i < cache->count; i++) {
        if (strcmp(cache->textures[i]->name, file_path) == 0) {
            return cache->textures[i]->texture;
        }
    }

    return load_texture(cache, renderer, file_path);
}

void
destroy_texture_cache(TextureCache* cache) {
    for (int i = 0; i < MAX_ASSET_FILES; i++) {
        SDL_DestroyTexture(cache->textures[i]->texture);
        cache->textures[i] = NULL;
        cache->count = 0;;
    }

}
