#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../../shared/logger.h"
#include "tilesheet.h"

//TODO: Figure out why I can't get SDL_GetBasePath()
//to work right
const char* tilesheet_paths[] = {
    [ABANDONED_MINES] = "../../assets/Tilesets/FG_Abandoned_Mines.png",
    [CELLAR_DOORS] = "../../assets/Tilesets/FG_Cellar_Doors.png",
    [CELLAR] = "../../assets/Tilesets/FG_Cellar.png",
    [DARK_CASTLE_ANIMATED] = "../../assets/Tilesets/FG_Dark_Castle_Animated.png",
    [DARK_CASTLE] = "../../assets/Tilesets/FG_Dark_Castle.png",
    [DARK_CASTLE_PURPLE_ANIMATED] = "../../assets/Tilesets/FG_Dark_Castle_Purple_Animated.png",
    [DARK_CASTLE_PURPLE] = "../../assets/Tilesets/FG_Dark_Castle.png",
    [EXTRAS] = "../../assets/Tilesets/FG_Extras.png",
    [FOREST_FALL] = "../../assets/Tilesets/FG_Forest_Fall.png",
    [FOREST_SPRING] = "../../assets/Tilesets/FG_Forest_Spring.png",
    [FOREST_SUMMER] = "../../assets/Tilesets/FG_Forest_Summer.png",
    [FOREST_WINTER] = "../../assets/Tilesets/FG_Forest_Winter.png",
    [FORTRESS] = "../../assets/Tilesets/FG_Fortress.png",
    [FORTRESS_WINTER] = "../../assets/Tilesets/FG_Fortress_Winter.png",
    [FROZEN_CAVES] = "../../assets/Tilesets/FG_Frozen_Caves.png",
    [GRASSLANDS_FALL] = "../../assets/Tilesets/FG_Grasslands_Fall.png",
    [GRASSLANDS_SPRING] = "../../assets/Tilesets/FG_Grasslands_Spring.png",
    [GRASSLANDS_SUMMER] = "../../assets/Tilesets/FG_Grasslands_Summer.png",
    [GRASSLANDS_WINTER] = "../../assets/Tilesets/FG_Grasslands_Winter.png",
    [GROUNDS] = "../../assets/Tilesets/FG_Grounds.png",
    [HOUSES] = "../../assets/Tilesets/FG_Houses.png",
    [HOUSES_WINTER] = "../../assets/Tilesets/FG_Houses_Winter.png",
    [INTERIOR] = "../../assets/Tilesets/FG_Interior.png",
    [MAGMA_DUNGEON] = "../../assets/Tilesets/FG_Magma_Dungeon.png",
    [MAGMA_DUNGEON_WARM] = "../../assets/Tilesets/FG_Magma_Dungeon_Warm.png",
    [MOUNTAINS_2] = "../../assets/Tilesets/FG_Mountains_2.png",
    [MOUNTAINS_2_WINTER] = "../../assets/Tilesets/FG_Mountains_2_Winter.png",
    [MOUNTAINS] = "../../assets/Tilesets/FG_Mountains.png",
    [MOUNTAINS_WINTER] = "../../assets/Tilesets/FG_Mountains_Winter.png",
    [VILLAGE] = "../../assets/Tilesets/FG_Village.png",
    [VILLAGE_WINTER] = "../../assets/Tilesets/FG_Village_Winter.png"
};

SDL_Texture* load_tilesheet(SDL_Renderer* renderer, tilesheet_t file_path, int* width, int* height, int* totalSheets) {
    const char* file = tilesheet_paths[file_path];
        
    SDL_Surface* surface = IMG_Load(file);
    if (surface == NULL) {
        GFATAL("Failed to load spritesheet: %s\n", IMG_GetError());
        return NULL;
    }

    *width = surface->w;
    *height = surface->h;
    *totalSheets = sizeof(tilesheet_paths) / sizeof(tilesheet_paths[0]);

    SDL_Texture* tilesheet = SDL_CreateTextureFromSurface(renderer, surface);
    if(tilesheet == NULL) {
        GFATAL("Failed to create spritesheet texture: %s\n", SDL_GetError());
        return NULL;
    }

    return tilesheet;
}

SDL_Texture* load_tilesheet_only(SDL_Renderer* renderer, tilesheet_t file_path) {
    const char* file = tilesheet_paths[file_path];
        
    SDL_Surface* surface = IMG_Load(file);
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

void free_tilesheet(SDL_Texture* tilesheet) {
    free(tilesheet);
}
