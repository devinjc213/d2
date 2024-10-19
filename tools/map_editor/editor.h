#ifndef EDITOR_H
#define EDITOR_H

#include <SDL2/SDL.h>
#include "texture_cache.h"
#include "tilemap.h"

typedef struct {
  int layer;
  int collidable;
  int render_layer_1;
  int render_layer_2;
  int render_layer_3;
} EditorSettings;

typedef struct {
  int quit;

  SDL_Window* e_win;
  SDL_Window* t_win;
  SDL_Window* s_win;

  SDL_Renderer* e_render;
  SDL_Renderer* t_render;
  SDL_Renderer* s_render;

  TextureCache* e_cache;
  TextureCache* t_cache;

  TileMap* tile_map;

  int mouse_held_down;

  int e_cur_m_x;
  int e_cur_m_y;
  int e_select_m_x;
  int e_select_m_y;
  
  int t_cur_m_x;
  int t_cur_m_y;
  int t_select_m_x;
  int t_select_m_y;
  
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

  SelectionBuffer* select_buf;

  EditorSettings settings;
} Editor;


void init_editor(Editor* e);
void init_windows(Editor* e);
void init_renderers(Editor* e);
void init_cache(Editor* e);
void init_textures(Editor* e);
void init_tile_map(Editor* e);
void init_settings(Editor* e);

#endif
