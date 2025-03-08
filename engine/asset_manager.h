#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H
#include <SDL2/SDL.h>

#define MAX_ASSET_FILES 500

enum Renderer {
  R_EDITOR,
  R_TILESHEETS,
  R_GAME
};

typedef struct AssetChild {
  SDL_Texture** asset;
  char* path;
  struct AssetChild* next_child;
} AssetChild;

typedef struct {
  int count;
  int capacity;
  AssetChild* assets[MAX_ASSET_FILES];
} AssetMap;

unsigned long hash(unsigned char *str);
void init_asset_map(AssetMap* map);

AssetChild* find_matching_child(AssetChild* child, char* path);
AssetChild* find_empty_child(AssetChild* child);
SDL_Texture* get_asset(AssetMap* map, char* path, enum Renderer renderer);
int add_asset(AssetMap* map, SDL_Texture** textures, int render_count, char* path);
int remove_asset(AssetMap* map, char* path);

SDL_Texture** load_asset_multi_render(SDL_Renderer** renderers, int render_count, char* file_path);
SDL_Texture* load_asset_to_window(SDL_Renderer* renderer, char *filename, int* width, int* height);
SDL_Texture* load_asset(SDL_Renderer* renderer, char *filename);
void free_asset(SDL_Texture* tilesheet);

#endif
