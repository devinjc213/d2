#ifndef TILEMAP_H
#define TILEMAP_H

#define MAX_TILES 32768

#include "../../shared/sprite.h"

typedef struct { 
  SDL_Rect rect;
  Sprite* sprite;
  int collidable;
  int layer;
} Tile;

typedef struct {
    int count;
    int max_size;
    Tile** tiles;
} TileMap;

TileMap* create_tilemap();
int add_tile(TileMap* tilemap, int x, int y, Sprite* sprite);
void remove_tile(TileMap* tilemap, Tile* tile_to_remove);
void free_tilemap(TileMap* tilemap);
Tile* find_tile_by_coords(TileMap* tilemap, int x, int y);

#endif
