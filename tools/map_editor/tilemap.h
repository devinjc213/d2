#ifndef TILEMAP_H
#define TILEMAP_H

#define MAX_TILES 32768

typedef struct {
    int count;
    int max_size;
    Block** blocks;
} TileMap;

typedef struct { 
    SDL_Rect rect;
    Sprite* sprite;
    int collidable;
    int layer;
} Tile;

TileMap* create_tilemap();
void add_tile(TileMap* tilemap, int x, int y, Sprite* sprite);
void remove_tile(TileMap* tilemap, Tile* tile_to_remove);

#endif
