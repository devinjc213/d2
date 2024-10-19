#ifndef TILESHEET_H
#define TILESHEET_H
#include <SDL2/SDL.h>

#define MAX_MAP_W 64
#define MAX_MAP_H 64

#define LAYERS 3

#define TILE_W 16
#define TILE_H 16

#define MAX_TILES_IN_MAP 12288
#define MAX_TILES_IN_LAYER 4096

typedef enum {
    ABANDONED_MINES,
    CELLAR_DOORS,
    CELLAR,
    DARK_CASTLE_ANIMATED,
    DARK_CASTLE,
    DARK_CASTLE_PURPLE_ANIMATED,
    DARK_CASTLE_PURPLE,
    EXTRAS,
    FOREST_FALL,
    FOREST_SPRING,
    FOREST_SUMMER,
    FOREST_WINTER,
    FORTRESS,
    FORTRESS_WINTER,
    FROZEN_CAVES,
    GRASSLANDS_FALL,
    GRASSLANDS_SPRING,
    GRASSLANDS_SUMMER,
    GRASSLANDS_WINTER,
    GROUNDS,
    HOUSES,
    HOUSES_WINTER,
    INTERIOR,
    MAGMA_DUNGEON,
    MAGMA_DUNGEON_WARM,
    MOUNTAINS_2,
    MOUNTAINS_2_WINTER,
    MOUNTAINS,
    MOUNTAINS_WINTER,
    VILLAGE,
    VILLAGE_WINTER
} tilesheet_t;

typedef struct {
  tilesheet_t tilesheet;
  SDL_Rect rect;
} SelectionBuffer;

SDL_Texture* load_tilesheet(SDL_Renderer* renderer, tilesheet_t filename, int* width, int* height, int* totalSheets);
SDL_Texture* load_tilesheet_only(SDL_Renderer* renderer, tilesheet_t filename);

void free_tilesheet(SDL_Texture* tilesheet);

#endif
