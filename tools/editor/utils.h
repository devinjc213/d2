#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>

typedef struct {
  float offset_x, offset_y;
  float scale;
} ZoomState;

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

int snap_to_grid(int coord, float scale);
void apply_zoom(ZoomState* z, float new_scale, int m_x, int m_y);
SDL_bool is_window_focused(SDL_Window* window);

unsigned long hash(unsigned char *str);

#endif
