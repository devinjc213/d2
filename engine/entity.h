#ifndef ENTITY_H
#define ENTITY_H
#include <SDL2/SDL.h>
#include "math/vector2.h"

typedef struct {
  SDL_Rect pos;
  SDL_Rect src_rect;

  char* src;

  float speed;

  int cur_frame;
  int frame_count;

  // for animations in spreadsheets that wrap
  int frame_rows;
  int frame_columns;

  float frame_duration;
  float frame_time_elapsed;

} Entity;

#endif
