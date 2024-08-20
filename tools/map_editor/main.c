#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "defs.h"
#include "../../shared/logger.h"
#include "tilemap.h"
#include "panel.h"
#include "../../shared/tilesheet.h"

SDL_Window* eWindow;
SDL_Renderer* eRenderer;
void cleanup_editor();
void render_grid(SDL_Renderer* renderer);
void render_tile(SDL_Renderer* renderer, Tile* tile);
int snap_to_grid(int coord, int tile_size);

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) != (IMG_INIT_PNG)) {
        SDL_Quit();
        return 1;
    }

    SDL_Window* eWindow = SDL_CreateWindow("Tilemap Editor",
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SCREEN_WIDTH,
                                           SCREEN_HEIGHT,
                                           SDL_WINDOW_SHOWN);

    SDL_Renderer* eRenderer = SDL_CreateRenderer(eWindow, -1, SDL_RENDERER_ACCELERATED);

    if (init_sprites(eRenderer, "../../assets/tileset.png", "../../tile_coords") > 0) {
        GFATAL("Failed to initialize sprites");
    }

    SDL_Texture* tilesheet = load_tilesheet(eRenderer, MOUNTAINS);
    Sprite* selected_sprite = NULL;
    TileMap* tilemap = create_tilemap();
    SpritePanel* panel = init_panel(&sprite_map);
    if (panel == NULL) {
        GWARN("Panel failed to initialize!");
    }

    int quit = 0;
    int mouse_held_down = 0;

    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        mouse_held_down = 1;

                        if (e.button.x > PANEL_WIDTH && selected_sprite != NULL) {
                            int snapped_x = snap_to_grid(e.button.x, 16);
                            int snapped_y = snap_to_grid(e.button.y, 16);
                            add_tile(tilemap, snapped_x, snapped_y, selected_sprite, 0, 0);
                        }
                        else if (e.button.x < PANEL_WIDTH) {
                            // add padding to account for 5 offset from start
                            int snapped_x = snap_to_grid(e.button.x, 21) + PANEL_PADDING;
                            int snapped_y = snap_to_grid(e.button.y, 21) + PANEL_PADDING;
                            Sprite* clicked_sprite = find_sprite_panel(panel, snapped_x, snapped_y);
                            if (clicked_sprite != NULL) {
                                selected_sprite = clicked_sprite;
                            }
                        }
                    }

                    if (e.button.button == SDL_BUTTON_RIGHT) {
                        int snapped_x = snap_to_grid(e.button.x, 16);
                        int snapped_y = snap_to_grid(e.button.y, 16);
                        Tile* tile_to_remove = find_tile_by_coords(tilemap, snapped_x, snapped_y);
                        if (tile_to_remove != NULL) {
                            remove_tile(tilemap, tile_to_remove);
                        } 
                    }
                    break;

                case SDL_MOUSEMOTION:
                    if (mouse_held_down) {
                        if (e.button.button == SDL_BUTTON_LEFT) {
                            int snapped_x = snap_to_grid(e.button.x, 16);
                            int snapped_y = snap_to_grid(e.button.y, 16);
                            if (snapped_x > PANEL_WIDTH) {
                                add_tile(tilemap, snapped_x, snapped_y, selected_sprite, 0, 0);
                            }
                        }
                    }

                    break;

                case SDL_MOUSEBUTTONUP:
                    mouse_held_down = 0;
                    break;  
            }
        }

        SDL_SetRenderDrawColor(eRenderer, 100, 100, 100, 255);
        SDL_RenderClear(eRenderer);

        render_grid(eRenderer);
        render_panel(eRenderer, spritesheet, panel, selected_sprite);

        for (int i = 0; i < tilemap->count; i++) {
            render_tile(eRenderer, tilemap->tiles[i]);
        }

        SDL_RenderPresent(eRenderer);
    }

    free_tilemap(tilemap);
    free_panel(panel);
    cleanup_editor();
    return 0;
}

void render_tile(SDL_Renderer* renderer, Tile* tile) {
    SDL_Rect dest = { tile->rect.x, tile->rect.y, TILE_SIZE, TILE_SIZE };

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    SDL_RenderCopy(renderer, spritesheet, &tile->sprite->rect, &dest);
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

int snap_to_grid(int coord, int tile_size) {
    return (coord / tile_size) * tile_size;
}

void cleanup_editor() {
    SDL_DestroyRenderer(eRenderer);
    SDL_DestroyWindow(eWindow);

    IMG_Quit();
    SDL_Quit();
}
