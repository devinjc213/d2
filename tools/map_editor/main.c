#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "defs.h"
#include "../../shared/logger.h"


SDL_Window* eWindow;
SDL_Renderer* eRenderer;
int mX, mY;
void cleanup_editor();
void render_block(SDL_Renderer* renderer, Block* block);
void render_grid(SDL_Renderer* renderer);
int snap_to_grid(int coord);
Block* find_block(Block blocks[], int block_count, int x, int y);
void remove_block(Block blocks[], int *block_count, Block* block_to_remove);

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

    if (init_sprites(eRenderer, "../../assets/tileset.png", "../../tile_coords") > 0) {
        GFATAL("Failed to initialize sprites");
    }

    Block blocks[16];
    Sprite* selected_sprite = find_sprite(&sprite_map, "flask_big_green");
    int block_count = 0;

    int quit = 0;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int snapped_x = snap_to_grid(e.button.x);
                int snapped_y = snap_to_grid(e.button.y);

                if (e.button.button == SDL_BUTTON_LEFT) {
                    blocks[block_count].rect.x = snapped_x;
                    blocks[block_count].rect.y = snapped_y;
                    blocks[block_count].sprite = selected_sprite;
                    blocks[block_count].collidable = 0;
                    blocks[block_count].layer = 0;
                    block_count++;
                }
                else if (e.button.button == SDL_BUTTON_RIGHT) {
                    Block* block_to_remove = find_block(blocks, block_count, snapped_x, snapped_y);
                    if (block_to_remove != NULL) {
                        remove_block(blocks, &block_count, block_to_remove);
                    } 
                }
            }
        }

        SDL_SetRenderDrawColor(eRenderer, 255, 255, 255, 255);
        SDL_RenderClear(eRenderer);

        render_grid(eRenderer);

        for (int i = 0; i < block_count; i++) {
            render_block(eRenderer, &blocks[i]);
        }

        SDL_RenderPresent(eRenderer);
    }

    cleanup_editor();
    return 0;
}

void render_block(SDL_Renderer* renderer, Block* block) {
    SDL_Rect dest = { block->rect.x, block->rect.y, TILE_SIZE, TILE_SIZE };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_RenderCopy(renderer, spritesheet, &block->sprite->rect, &dest);
}

void render_grid(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 83, 83, 83, 155);

    for (int i = 16; i < SCREEN_WIDTH; i += 16) {
        SDL_RenderDrawLine(renderer, i, 0, i, SCREEN_HEIGHT);
    }

    for (int i = 16; i < SCREEN_HEIGHT; i += 16) {
        SDL_RenderDrawLine(renderer, 0, i, SCREEN_WIDTH, i);
    }
}

Block* find_block(Block blocks[], int block_count, int x, int y) {
    for (int i = 0; i < block_count; i++) {
        if (blocks[i].rect.x == x && blocks[i].rect.y == y) {
            return &blocks[i];
        }
    }

    return NULL;
}

void remove_block(Block blocks[], int *block_count, Block *block_to_remove) {
    int index = block_to_remove - blocks;
    if (index >= 0 && index < *block_count) {
        blocks[index] = blocks[*block_count - 1];
        (*block_count)--;
    }
}

int snap_to_grid(int coord) {
    return (coord / 16) * 16;
}

void cleanup_editor() {
    SDL_DestroyRenderer(eRenderer);
    SDL_DestroyWindow(eWindow);

    IMG_Quit();
    SDL_Quit();
}
