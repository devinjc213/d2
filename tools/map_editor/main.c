#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "defs.h"

typedef struct { 
    int x, y;
} Block;

SDL_Window* eWindow;
SDL_Renderer* eRenderer;
int mX, mY;
void cleanup_editor();
void render_block(SDL_Renderer* renderer, int x, int y);

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) != (IMG_INIT_PNG)) {
        SDL_Quit();
        return 1;
    }

    SDL_Window* eWindow = SDL_CreateWindow("Tilemap Editor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* eRenderer = SDL_CreateRenderer(eWindow, -1, SDL_RENDERER_ACCELERATED);

    Block blocks[16];
    int block_count = 0;

    int quit = 0;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    blocks[block_count].x = e.button.x;
                    blocks[block_count].y = e.button.y;
                    block_count++;
                }
            }
        }

        SDL_SetRenderDrawColor(eRenderer, 0, 0, 0, 255);
        SDL_RenderClear(eRenderer);

        for (int i = 0; i < block_count; i++) {
            render_block(eRenderer, blocks[i].x, blocks[i].y);
        }

        SDL_RenderPresent(eRenderer);
    }

    cleanup_editor();
    return 0;
}

void render_block(SDL_Renderer* renderer, int x, int y) {
    SDL_Rect rect = { x, y, TILE_SIZE, TILE_SIZE };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_RenderFillRect(renderer, &rect);
}

void cleanup_editor() {
    SDL_DestroyRenderer(eRenderer);
    SDL_DestroyWindow(eWindow);

    IMG_Quit();
    SDL_Quit();
}
