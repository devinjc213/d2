#ifndef EDITOR_H
#define EDITOR_H

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#include <SDL2/SDL.h>
#include "texture_cache.h"
#include "defs.h"
#include "tilemap.h"

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
  int quit;

  SDL_Window* e_win;
  SDL_Window* t_win;
  SDL_Window* s_win;

  SDL_Renderer* e_render;
  SDL_Renderer* t_render;
  SDL_Renderer* s_render;

  struct nk_context* nk_ctx;
  struct nk_font_atlas* nk_atlas;

  TextureCache* e_cache;
  TextureCache* t_cache;

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

//input
void screen_to_tilesheet(int w,
                         int h,
                         int zoom_x,
                         int zoom_y,
                         float scale,
                         int screen_x,
                         int screen_y,
                         int* tile_x,
                         int* tile_y);
//rendering
void tilesheet_to_screen(int zoom_x,
                         int zoom_y,
                         float scale,
                         int tile_x,
                         int tile_y,
                         int* screen_x,
                         int* screen_y);

void apply_zoom(ZoomState* z, float new_scale, int m_x, int m_y);
SDL_bool is_window_focused(SDL_Window* window);
int snap_to_grid(int coord, float scale);
void render_grid(SDL_Renderer* renderer, int offset_x, int offset_y, float scale);
void render_editor_win(Editor* e);
void render_tilesheet_win(Editor* e);
void render_settings_win(Editor* e);

#endif
