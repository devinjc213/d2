#ifndef EDITOR_H
#define EDITOR_H

#include <SDL2/SDL.h>
#include "tilemap.h"
#include "defs.h"
#include "../../engine/asset_manager.h"

typedef struct {
  char *paths[MAX_ASSET_FILES];
  int count;
} asset_directory_t;

typedef struct {
  int layer;
  int collidable;
  int render_layer_1;
  int render_layer_2;
  int render_layer_3;
} EditorSettings;

typedef struct {
  int x;
  int y;
  int click_x;
  int click_y;
  int left_pressed;
  int middle_pressed;
  int right_pressed;
} MouseState;

typedef struct {
  char *tilesheet;
  SDL_Rect rect;
  int active_selection;
} SelectionBuffer;

typedef struct {
  int quit;

  SDL_Window* e_win;
  SDL_Window* t_win;
  SDL_Window* s_win;

  SDL_Renderer* e_render;
  SDL_Renderer* t_render;
  SDL_Renderer* s_render;

  struct nk_sdl* e_nk_state;
  struct nk_sdl* t_nk_state;
  struct nk_sdl* s_nk_state;
  struct nk_font_atlas* e_nk_atlas;
  struct nk_font_atlas* t_nk_atlas;
  struct nk_font_atlas* s_nk_atlas;

  asset_directory_t d_asset_dir;

  AssetMap* asset_map;

  TileMap* tile_map;

  ZoomState e_zoom;
  ZoomState t_zoom;

  int mouse_held_down;

  MouseState e_mouse;
  MouseState t_mouse;
  MouseState s_mouse;

  int e_x;
  int e_y;
  int e_w;
  int e_h;

  int t_x;
  int t_y;
  int t_w;
  int t_h;

  int s_x;
  int s_y;
  int s_w;
  int s_h;

  int cur_sheet_index;
  int total_sheets;
  SDL_Texture* cur_e_sheet;
  SDL_Texture* cur_t_sheet;

  SelectionBuffer select_buf;

  EditorSettings settings;
} Editor;

void init_editor(Editor* e);
void cleanup_editor(Editor* e);
void render_grid(SDL_Renderer* renderer, int offset_x, int offset_y, float scale);
void render_editor_win(Editor* e);
void render_tilesheet_win(Editor* e);
void render_settings_win(Editor* e);
void handle_settings_input(SDL_Event* e, Editor* editor);

#endif
