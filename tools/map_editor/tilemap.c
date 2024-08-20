#include <SDL2/SDL.h>
#include <stdlib.h>
#include "tilemap.h"
#include "../../shared/logger.h"
#include "defs.h"

TileMap* create_tilemap() {
    TileMap* map = malloc(sizeof(TileMap));
    map->count = 0;
    map->max_size = MAX_TILES;
    map->tiles = malloc(sizeof(Tile) * MAX_TILES);
    
    return map;
}

int add_tile(TileMap* tilemap, int x, int y, Sprite* sprite, int collidable, int layer) {
    if (tilemap->count >= tilemap->max_size) {
        GWARN("Reached maximum tile limit");
        return 1;
    }

    // check for existing tile in this location first
    // if a different sprite, we can just update the same block
    //
    // Bug/feature: will still paint over the last tile
    Tile* found_tile = find_tile_by_coords(tilemap, x, y);
    if (found_tile != NULL && found_tile->sprite != sprite) {
        found_tile->sprite = sprite;
        return 0;
    }

    Tile* new_tile = malloc(sizeof(Tile));
    new_tile->sprite = sprite;
    new_tile->rect.x = x;
    new_tile->rect.y = y;
    new_tile->collidable = collidable;
    new_tile->layer = layer;
    tilemap->tiles[tilemap->count++] = new_tile;
    return 0;
}

Tile* find_tile_by_coords(TileMap* tilemap, int x, int y) {
    for (int i = 0; i < tilemap->count; i++) {
        if (tilemap->tiles[i]->rect.x == x && tilemap->tiles[i]->rect.y == y) {
            return tilemap->tiles[i];
        }
    }

    return NULL;
}

void remove_tile(TileMap* tilemap, Tile* tile_to_remove) {
    for (int i = 0; i < tilemap->count; i++) {
        if (tilemap->tiles[i] == tile_to_remove) {
            free(tile_to_remove);
            tilemap->tiles[i] = tilemap->tiles[--tilemap->count];
            return;
        }
    }
}

void free_tilemap(TileMap* tilemap) {
    for (int i = 0; i < tilemap->count; i++) {
        free(tilemap->tiles[i]);
    }

    free(tilemap->tiles);
    free(tilemap);
}
