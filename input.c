#include "input.h"

void init_input(InputState* input) {
    input->up = 0;
    input->down = 0;
    input->left = 0;
    input->right = 0;
    input->attack = 0;
}

void handle_input(SDL_Event* event, InputState* input) {
    switch (event->type) {
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_w:
                    input->up = 1;
                    break;
                case SDLK_s:
                    input->down = 1;
                    break;
                case SDLK_a:
                    input->left = 1;
                    break;
                case SDLK_d:
                    input->right = 1;
                    break;
                case SDLK_SPACE:
                    input->attack = 1;
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (event->key.keysym.sym) {
                case SDLK_w:
                    input->up = 0;
                    break;
                case SDLK_s:
                    input->down = 0;
                    break;
                case SDLK_a:
                    input->left = 0;
                    break;
                case SDLK_d:
                    input->right = 0;
                    break;
                case SDLK_SPACE:
                    input->attack = 0;
                    break;
            }
            break;
    }
}
