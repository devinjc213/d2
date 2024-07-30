#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

typedef struct {
    int up;
    int down;
    int left;
    int right;
    int attack;
}  InputState;

void init_input(InputState* input);
void handle_input(SDL_Event* event, InputState* input);

#endif
