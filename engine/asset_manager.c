#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../shared/logger.h"
#include "asset_manager.h"

unsigned long
hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

void
init_asset_map(AssetMap* map) {
    map->capacity = MAX_ASSET_FILES;
    map->count = 0;

    for (int i = 0; i < MAX_ASSET_FILES; i++) {
        map->assets[i] = NULL;
    }
}

AssetChild*
find_empty_child(AssetChild* child) {
    AssetChild* temp = child;

    while (temp->next_child != NULL) {
        temp = temp->next_child;
    }

    return temp;
}

AssetChild*
find_matching_child(AssetChild* child, char* path) {
    AssetChild* temp = child;

    while (temp->path != path) {
        temp = temp->next_child;
    }

    return temp;
}


int
add_asset(AssetMap* map, SDL_Texture** textures, int render_count, char* path) {
    int index = hash((unsigned char*)path) % MAX_ASSET_FILES;

    if (map->count < MAX_ASSET_FILES) {
        if (map->assets[index] == NULL) {
            map->assets[index] = malloc(sizeof(AssetChild));
            map->assets[index]->asset = malloc(render_count * sizeof(SDL_Texture*));
            for (int i = 0; i < render_count; i++) {
                map->assets[index]->asset[i] = textures[i];
            }
            map->assets[index]->path = strdup(path);
            map->assets[index]->next_child = NULL;
            map->count++;

            return index;
        }

        AssetChild* last = find_empty_child(map->assets[index]);
        last->next_child = malloc(sizeof(AssetChild));
        last->next_child->asset = malloc(render_count * sizeof(SDL_Texture*));
        for (int i = 0; i < render_count; i++) {
            last->next_child->asset[i] = textures[i];
        }
        last->next_child->path = strdup(path);
        map->count++;

        return index;
    }

    GINFO("Asset cache is full.  Haven't implemented realloc yet :(");
    return -1;
}

SDL_Texture*
get_asset(AssetMap* map, char* path, enum Renderer renderer) {
    int index = hash((unsigned char*)path) % MAX_ASSET_FILES;

    if (map->assets[index] == NULL) {
        GINFO("No assets at index %d for path %s", index, path);
        return NULL;
    }

    if (strcmp(map->assets[index]->path, path) == 0) {
        return map->assets[index]->asset[renderer];
    }
    
    AssetChild* match = find_matching_child(map->assets[index], path);

    if (match->asset == NULL) {
        GINFO("failed to find asset %s", path);
    }

    return match->asset[renderer];
}

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

    SDL_FreeSurface(surface);
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

    SDL_FreeSurface(surface);
    return tilesheet;
}

SDL_Texture**
load_asset_multi_render(SDL_Renderer** renderers, int render_count, char* file_path) {
    SDL_Surface* surface = IMG_Load(file_path);
    if (surface == NULL) {
        GFATAL("Failed to load spritesheet: %s\n", IMG_GetError());
        return NULL;
    }

    SDL_Texture** textures = malloc(render_count * sizeof(SDL_Texture*));

    for (int i = 0; i < render_count; i++) {
        textures[i] = SDL_CreateTextureFromSurface(renderers[i], surface);

        if(textures[i] == NULL) {
            GFATAL("Failed to create spritesheet texture: %s\n", SDL_GetError());
        }
    }

    SDL_FreeSurface(surface);
    return textures;
}

void free_asset(SDL_Texture* tilesheet) {
    free(tilesheet);
}
