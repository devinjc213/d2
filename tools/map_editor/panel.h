#ifndef PANEL_H
#define PANEL_H

#include <SDL2/SDL.h>
#include "../../shared/sprite.h"

#define PANEL_COLS 15
#define PANEL_ROWS 50
#define PANEL_PADDING 5
#define PANEL_WIDTH 400

// I hate this but refactor me later
typedef struct {
  int x;
  int y;
  Sprite* sprite;
} SpriteCoord; 

typedef struct {
    SDL_Rect rect;
    SpriteCoord** sprites;
    int sprite_count;
    Sprite* selected_sprite;
} SpritePanel;

SpritePanel* init_panel(SpriteMap* sprite_map);
Sprite* find_sprite_panel(SpriteCoord** coords, int x, int y);
void render_panel(SDL_Renderer* renderer,
                  SDL_Texture* spritesheet,
                  SpritePanel* panel, 
                  Sprite* selected_sprite);

#endif
