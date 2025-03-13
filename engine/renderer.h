#ifndef RENDERER_H
#define RENDERER_H
#include <SDL2/SDL.h>
#include "entity.h"
#include "math/vector2.h"

typedef struct {
  Vector2 pos;
  float w, h;
} RenderRect;

void render_ent(SDL_Renderer* renderer, Entity* ent);

#endif 
