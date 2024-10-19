#include <SDL2/SDL.h>
#include "editor.h"
#include "defs.h"
#include "../../shared/logger.h"

//TODO: check for errors lol

void init_editor(Editor* e) {
    init_windows(e);
    init_renderers(e);
    init_cache(e);
    init_textures(e);
    init_tile_map(e);
    init_settings(e);
}

void init_windows(Editor* e) {
    GINFO("Initializing windows...");
    e->quit = 0;

    e->e_win = SDL_CreateWindow("Tilemap Editor",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                SCREEN_WIDTH,
                                SCREEN_HEIGHT,
                                SDL_WINDOW_SHOWN);
    SDL_GetWindowPosition(e->e_win, &e->e_x, &e->e_y);
    SDL_GetWindowSize(e->e_win, &e->e_w, &e->e_h);

    e->t_win = SDL_CreateWindow("Tilesheets",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                500,
                                900,
                                SDL_WINDOW_SHOWN);
    SDL_SetWindowPosition(e->t_win, e->e_x + e->e_w + 10, e->e_y - 100);

    e->s_win = SDL_CreateWindow("Settings",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                300,
                                768,
                                SDL_WINDOW_SHOWN);
    SDL_SetWindowPosition(e->s_win, e->e_x - 310, e->e_y - 100);
}

void init_renderers(Editor* e) {
    e->e_render = SDL_CreateRenderer(e->e_win, -1, SDL_RENDERER_ACCELERATED);
    e->t_render = SDL_CreateRenderer(e->t_win, -1, SDL_RENDERER_ACCELERATED);
    e->s_render = SDL_CreateRenderer(e->s_win, -1, SDL_RENDERER_ACCELERATED);
}

void init_cache(Editor* e) {
    GINFO("Initializing cache...");
    e->e_cache = malloc(sizeof(TextureCache));
    e->t_cache = malloc(sizeof(TextureCache));
    init_texture_cache(e->e_cache);
    init_texture_cache(e->t_cache);
    GINFO("Cache intialized");
}

void init_textures(Editor* e) {
    GINFO("Initialzing textures...");
    e->cur_sheet_index = 0;
    // TODO: read a dir rather than hard code
    e->total_sheets = 31;
    e->cur_e_sheet = load_texture(e->e_cache, e->e_render, e->cur_sheet_index);
    e->cur_t_sheet = load_texture(e->t_cache, e->t_render, e->cur_sheet_index);
    SDL_QueryTexture(e->cur_t_sheet, NULL, NULL, &e->t_w, &e->t_h);
    SDL_SetWindowSize(e->t_win, e->t_w, e->t_h);
    GINFO("Textures initialized");
}

void init_tile_map(Editor* e) {
    GINFO("Initializing tile map...");
    e->tile_map = malloc(sizeof(TileMap));
    e->tile_map = create_tilemap(64, 64, 3);
    e->select_buf = malloc(sizeof(SelectionBuffer));
    GINFO("Tile map initialized");
}

void init_settings(Editor* e) {
    GINFO("Initializing settings...");
    e->e_select_m_x = 0;
    e->e_select_m_y = 0;
    e->t_select_m_x = 0;
    e->t_select_m_y = 0;
    e->e_cur_m_x = 0;
    e->e_cur_m_y = 0;
    e->t_cur_m_x = 0;
    e->t_cur_m_y = 0;
    e->mouse_held_down = 0;
    e->settings.layer = 0;
    e->settings.collidable = 0;
    e->settings.render_layer_1 = 1;
    e->settings.render_layer_2 = 1;
    e->settings.render_layer_3 = 1;
    GINFO("Settings initialized");
}
