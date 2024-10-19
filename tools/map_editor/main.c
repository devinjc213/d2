#define NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "../../shared/nuklear.h"

#define NK_SDL_RENDERER_IMPLEMENTATION
#include "../../shared/nuklear_sdl_renderer.h"

#include <SDL2/SDL.h>
#include <stdint.h>
#include <SDL2/SDL_image.h>
#include "defs.h"
#include "../../shared/logger.h"
#include "tilemap.h"
#include "tilesheet.h"
#include "texture_cache.h"


SDL_Window *eWindow, *tWindow, *sWindow;
SDL_Renderer *eRenderer, *tRenderer, *sRenderer;
int editorX, editorY, editorW, editorH;
struct nk_context *nk_ctx;

void cleanup_editor();
void render_grid(SDL_Renderer* renderer);
void get_texture_size(SDL_Texture* texture, int w, int h);
SDL_bool is_window_focused(SDL_Window* window);
int snap_to_grid(int coord);

int layer, collidable;
SelectionBuffer selectionBuf;

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) != (IMG_INIT_PNG)) {
        SDL_Quit();
        return 1;
    }

    SDL_Window* eWindow = SDL_CreateWindow("Tilemap Editor",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           SCREEN_WIDTH,
                                           SCREEN_HEIGHT,
                                           SDL_WINDOW_SHOWN);
    SDL_GetWindowPosition(eWindow, &editorX, &editorY);
    SDL_GetWindowSize(eWindow, &editorW, &editorH);

    SDL_Window* tWindow = SDL_CreateWindow("Tilesheets",
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           500,
                                           900,
                                           SDL_WINDOW_SHOWN);
    SDL_SetWindowPosition(tWindow, editorX + editorW + 10, editorY - 100);

    SDL_Window* sWindow = SDL_CreateWindow("Settings",
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           300,
                                           768,
                                           SDL_WINDOW_SHOWN);
    SDL_SetWindowPosition(sWindow, editorX - 310, editorY - 100);

    SDL_Renderer* eRenderer = SDL_CreateRenderer(eWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_Renderer* tRenderer = SDL_CreateRenderer(tWindow, -1, SDL_RENDERER_ACCELERATED);
    SDL_Renderer* sRenderer = SDL_CreateRenderer(sWindow, -1, SDL_RENDERER_ACCELERATED);

    GINFO("Initializing cache...");
    TextureCache *editor_cache, *tilesheet_cache;
    editor_cache = malloc(sizeof(TextureCache));
    tilesheet_cache = malloc(sizeof(TextureCache));
    init_texture_cache(editor_cache);
    init_texture_cache(tilesheet_cache);
    GINFO("Cache intialized");

    GINFO("Initilizing nuklear...");
    nk_ctx = nk_sdl_init(sWindow, sRenderer);
    struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    nk_sdl_font_stash_end();
    GINFO("nuklear initialized");

    int imageWidth, imageHeight, totalSheets = 31;
    int curTilesheetIndex = 0;
    SDL_Texture* curTilesheet = load_texture(tilesheet_cache, tRenderer, curTilesheetIndex);
    SDL_Texture* selectionTilesheet = load_texture(editor_cache, eRenderer, curTilesheetIndex);

    SDL_SetWindowSize(tWindow, imageWidth, imageHeight);

    Sprite* selected_sprite = NULL;
    TileMap* tilemap = create_tilemap(64, 64, 3);

    int quit = 0;
    int mouse_held_down = 0;
    int selectionBeginX, selectionBeginY, selectionCurX, selectionCurY, editorCurX, editorCurY;

    while (!quit) {
        SDL_Event e;
        nk_input_begin(nk_ctx);

        while (SDL_PollEvent(&e) != 0) {
            if (e.window.windowID == SDL_GetWindowID(sWindow)) {
                nk_sdl_handle_event(&e);
            }

            switch (e.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (is_window_focused(tWindow)) {
                        mouse_held_down = 1;
                        SDL_GetMouseState(&selectionBeginX, &selectionBeginY);
                    }

                    if (is_window_focused(eWindow)) {

                    }

                    break;

                case SDL_MOUSEMOTION:
                    if (is_window_focused(tWindow)) {
                        SDL_GetMouseState(&selectionCurX, &selectionCurY);
                    }

                    if (is_window_focused(eWindow)) {
                        SDL_GetMouseState(&editorCurX, &editorCurY);
                    }

                    break;

                case SDL_MOUSEBUTTONUP:
                    if (is_window_focused(tWindow)) {
                        int snapBX = snap_to_grid(selectionBeginX);
                        int snapBY = snap_to_grid(selectionBeginY);
                        int snapCX = snap_to_grid(selectionCurX);
                        int snapCY = snap_to_grid(selectionCurY);
                        SDL_Rect selection = {snapBX, snapBY, snapCX - snapBX, snapCY - snapBY};

                        selectionBuf.rect = selection;
                        selectionBuf.tilesheet = curTilesheetIndex;

                        mouse_held_down = 0;
                        selectionBeginX = 0;
                        selectionBeginY = 0;
                    }

                    break;  

                case SDL_KEYDOWN:
                    if ((e.key.keysym.sym == SDLK_RIGHT || e.key.keysym.sym == SDLK_LEFT) && is_window_focused(tWindow)) {
                        switch (e.key.keysym.sym) {
                            case SDLK_RIGHT:
                                curTilesheetIndex = (curTilesheetIndex + 1) % totalSheets;
                                break;
                            case SDLK_LEFT:
                                curTilesheetIndex = (curTilesheetIndex - 1 + totalSheets) % totalSheets;
                                break;
                        }
                        curTilesheet = get_texture(tilesheet_cache, tRenderer, curTilesheetIndex);
                        selectionTilesheet = get_texture(editor_cache, eRenderer, curTilesheetIndex);

                        int w, h;
                        SDL_QueryTexture(curTilesheet, NULL, NULL, &w, &h);

                        GINFO("setting window size, w: %d, h: %d", w, h);
                        SDL_SetWindowSize(tWindow, w, h);
                    }
            }
        }

        nk_input_end(nk_ctx);

        SDL_SetRenderDrawColor(eRenderer, 100, 100, 100, 255);
        SDL_SetRenderDrawColor(tRenderer, 100, 100, 100, 255);
        SDL_SetRenderDrawColor(sRenderer, 100, 100, 100, 255);

        if (nk_begin(nk_ctx,
                     "Settings",
                     nk_rect(0, 0, 300, 768),
                     NK_WINDOW_BORDER|NK_WINDOW_TITLE)
        ) {
            nk_layout_row_dynamic(nk_ctx, 30, 3);
            if (nk_option_label(nk_ctx, "Layer 1", layer == 0)) layer = 0;
            if (nk_option_label(nk_ctx, "Layer 2", layer == 1)) layer = 1;
            if (nk_option_label(nk_ctx, "Layer 3", layer == 2)) layer = 2;

            nk_layout_row_dynamic(nk_ctx, 30, 1);
            nk_bool check = collidable;
            if(nk_checkbox_label(nk_ctx, "Collidable", &check)) collidable = check;
        }
        nk_end(nk_ctx);

        SDL_RenderClear(eRenderer);
        SDL_RenderClear(tRenderer);
        SDL_RenderClear(sRenderer);

        render_grid(eRenderer);
        SDL_RenderCopy(tRenderer, curTilesheet, NULL, NULL);

        if (selectionBeginX && selectionBeginY) {
            int snapBX = snap_to_grid(selectionBeginX);
            int snapBY = snap_to_grid(selectionBeginY);
            int snapCX = snap_to_grid(selectionCurX);
            int snapCY = snap_to_grid(selectionCurY);
            SDL_SetRenderDrawBlendMode(tRenderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(tRenderer, 0, 0, 255, 100);
            SDL_Rect selection = {snapBX, snapBY, snapCX - snapBX, snapCY - snapBY};
            SDL_RenderFillRect(tRenderer, &selection);
            SDL_RenderDrawRect(tRenderer, &selection);
        }

        render_grid(tRenderer);
        nk_sdl_render(NK_ANTI_ALIASING_ON);

        if (is_window_focused(eWindow)) {
            int snapX = snap_to_grid(editorCurX);
            int snapY = snap_to_grid(editorCurY);

            SDL_Rect dest = {snapX, snapY, selectionBuf.rect.w, selectionBuf.rect.h};
            if (selectionTilesheet != NULL) {
                SDL_SetRenderDrawBlendMode(eRenderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(eRenderer, 0, 0, 255, 100);
                SDL_RenderFillRect(eRenderer, &dest);
                SDL_RenderDrawRect(eRenderer, &dest);
                SDL_RenderCopy(eRenderer, selectionTilesheet, &selectionBuf.rect, &dest);
            }
        }

        SDL_RenderPresent(eRenderer);
        SDL_RenderPresent(tRenderer);
        SDL_RenderPresent(sRenderer);
    }

    destroy_texture_cache(tilesheet_cache);
    destroy_texture_cache(editor_cache);
    cleanup_editor();
    return 0;
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

int snap_to_grid(int coord) {
    return (coord / 16) * 16;
}

SDL_bool is_window_focused(SDL_Window* window) {
    uint32_t flags = SDL_GetWindowFlags(window);
    return (flags & SDL_WINDOW_INPUT_FOCUS) != 0;
}

void cleanup_editor() {
    SDL_DestroyRenderer(eRenderer);
    SDL_DestroyWindow(eWindow);

    IMG_Quit();
    SDL_Quit();
}
