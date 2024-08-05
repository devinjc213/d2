#include <SDL2/SDL.h>
#include <stdlib.h>
#include "tilemap.h"
#include "defs.h"

TileMap* create_tilemap() {
    TileMap* map = malloc(sizeof(TileMap));
    map->count = 0;
    map->max_size = MAX_TILES
    map->blocks = malloc(sizeof(Tile) * MAX_TILES);
    

}

void add_tile(TileMap* tilemap, int x, int y, Sprite* sprite) {

}

void remove_tile(TileMap* tilemap, Tile* tile_to_remove) {

}
