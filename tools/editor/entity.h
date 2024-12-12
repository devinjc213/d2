#ifndef ENTITY_H
#define ENTITY_H

typedef struct {
  int x;
  int y;
  int w;
  int h;

  // TODO: I want to save the selected entity
  // source image as its own image, rather than 
  // loading in the whole tilesheet every use,
  // but we'll do that later
  char *src;

  int src_x;
  int src_y;
  int src_w;
  int src_h;

  int cur_frame;
  int frame_count;

  // for animations in spreadsheets that wrap
  int frame_rows;
  int frame_columns;

  float frame_duration;
  float time_elapsed;

} Entity;

#endif
