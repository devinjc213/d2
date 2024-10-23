#include <SDL2/SDL.h>
#include <stdlib.h>
#include "tilemap.h"
#include "../../shared/logger.h"
#include "defs.h"
#include "texture_cache.h"

TileMap* create_tilemap(int height, int width, int layers) {
    TileMap* map = malloc(sizeof(TileMap));
    map->height = height;
    map->width = width;
    map->layer_count = layers; 
    map->prop_count = 0;
    map->props_layer = malloc(height * width * sizeof(PropTile));
    map->render_layers = malloc(layers * sizeof(RenderLayer));

    for (int i = 0; i < layers; i++) {
        init_render_layer(&map->render_layers[i], INITIAL_LAYER_CAPACITY);
    }
    
    return map;
}

void init_render_layer(RenderLayer* render_layer, int initial_capacity) {
    render_layer->count = 0;
    render_layer->capacity = initial_capacity;
    render_layer->tiles = malloc(initial_capacity * sizeof(RenderTile));
}

void add_render_tile(RenderLayer* layer, RenderTile tile) {
    if (layer->count == layer->capacity) {
        layer->capacity *= 2;
        layer->tiles = realloc(layer->tiles, layer->capacity * sizeof(RenderTile));
    }

    layer->tiles[layer->count++] = tile;
}

void add_prop_tile(TileMap* map, PropTile props) {
    if (map->prop_count == map->height * map->width) {
        GERROR("Prop tiles full");
    }

    map->props_layer[map->prop_count++] = props;
}


void render_layer(RenderLayer* layer, SDL_Renderer* renderer, TextureCache* cache) {
    if (!layer->count) return;

    for (int i = 0; i < layer->count; i++) {
        SDL_Texture* texture = get_texture(cache, renderer, layer->tiles[i].tilesheet);
        SDL_RenderCopy(renderer, texture, &layer->tiles[i].src, &layer->tiles[i].dest);
    }
}
