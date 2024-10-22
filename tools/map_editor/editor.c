#include <SDL2/SDL.h>
#include "editor.h"
#include "defs.h"
#include "../../shared/logger.h"

static void init_windows(Editor* e);
static void init_renderers(Editor* e);
static void init_cache(Editor* e);
static void init_textures(Editor* e);
static void init_tile_map(Editor* e);
static void init_settings(Editor* e);
//TODO: check for errors lol

void init_editor(Editor* e) {
    init_windows(e);
    init_renderers(e);
    init_cache(e);
    init_textures(e);
    init_tile_map(e);
    init_settings(e);
}

void i_screen_to_tilesheet(Editor* e,
                           int zoom_x,
                           int zoom_y,
                           float scale,
                           int screen_x,
                           int screen_y,
                           int* tile_x,
                           int* tile_y) {
    *tile_x = (screen_x - zoom_x) / scale;
    *tile_y = (screen_y - zoom_y) / scale;

    //clamp
    *tile_x = (*tile_x < 0) ? 0 : (*tile_x >= e->t_w) ? e->t_w - 1 : *tile_x;
    *tile_y = (*tile_y < 0) ? 0 : (*tile_y >= e->t_h) ? e->t_h - 1 : *tile_y;
}

void r_tilesheet_to_screen(Editor* e,
                           int zoom_x,
                           int zoom_y,
                           float scale,
                           int tile_x,
                           int tile_y,
                           int* screen_x,
                           int* screen_y) {
    *screen_x = (tile_x * scale) + zoom_x;
    *screen_y = (tile_y * scale) + zoom_y;
}

int snap_to_grid(int coord, float scale) {
    float scaled_tile = TILE_SIZE * scale;

    return (int)(floorf(coord / scaled_tile) * scaled_tile);
}

void apply_zoom(ZoomState* z, float new_scale, int m_x, int m_y) {
    float old_scale = z->scale;
    z->scale = new_scale;

    float world_x = (m_x - z->offset_x) / old_scale;
    float world_y = (m_y - z->offset_y) / old_scale;

    z->offset_x = snap_to_grid(m_x - (world_x * new_scale), new_scale);
    z->offset_y = snap_to_grid(m_y - (world_y * new_scale), new_scale);

    if (fabsf(new_scale - 1.0f) < 0.001f) {
        z->offset_x = 0;
        z->offset_y = 0;
    }

    GINFO("%f", new_scale);
    GINFO("offset x: %f, y: %f, scale: %f", z->offset_x, z->offset_y, z->scale);
}

SDL_bool is_window_focused(SDL_Window* window) {
    uint32_t flags = SDL_GetWindowFlags(window);
    return (flags & SDL_WINDOW_INPUT_FOCUS) != 0;
}

void render_grid(SDL_Renderer* renderer, int offset_x, int offset_y, float scale) {
    SDL_SetRenderDrawColor(renderer, 83, 83, 83, 155);

    float tile_size_scaled = TILE_SIZE * scale;

    float start_x = -fmodf(offset_x, tile_size_scaled);
    if (start_x > 0) start_x -= tile_size_scaled;

    float start_y = -fmodf(offset_y, tile_size_scaled);
    if (start_y > 0) start_y -= tile_size_scaled;

    for (float x = start_x; x < SCREEN_WIDTH; x += tile_size_scaled) {
        SDL_RenderDrawLine(renderer, (int)x, 0, (int)x, SCREEN_HEIGHT);
    }

    for (float y = start_y; y < SCREEN_HEIGHT; y += tile_size_scaled) {
        SDL_RenderDrawLine(renderer, 0, (int)y, SCREEN_WIDTH, (int)y);
    }
}

static void init_windows(Editor* e) {
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

static void init_renderers(Editor* e) {
    e->e_render = SDL_CreateRenderer(e->e_win, -1, SDL_RENDERER_ACCELERATED);
    e->t_render = SDL_CreateRenderer(e->t_win, -1, SDL_RENDERER_ACCELERATED);
    e->s_render = SDL_CreateRenderer(e->s_win, -1, SDL_RENDERER_ACCELERATED);
}

static void init_cache(Editor* e) {
    GINFO("Initializing cache...");
    e->e_cache = malloc(sizeof(TextureCache));
    e->t_cache = malloc(sizeof(TextureCache));
    init_texture_cache(e->e_cache);
    init_texture_cache(e->t_cache);
    GINFO("Cache intialized");
}

static void init_textures(Editor* e) {
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

static void init_tile_map(Editor* e) {
    GINFO("Initializing tile map...");
    e->tile_map = malloc(sizeof(TileMap));
    e->tile_map = create_tilemap(64, 64, 3);
    e->select_buf = malloc(sizeof(SelectionBuffer));
    GINFO("Tile map initialized");
}

static void init_settings(Editor* e) {
    GINFO("Initializing settings...");
    e->e_zoom.offset_x = 0;
    e->e_zoom.offset_y = 0;
    e->e_zoom.scale = 1.0f;
    e->t_zoom.offset_x = 0;
    e->t_zoom.offset_y = 0;
    e->t_zoom.scale = 1.0f;

    e->e_mouse.x = 0;
    e->e_mouse.y = 0;
    e->e_mouse.click_x = 0;
    e->e_mouse.click_y = 0;
    e->e_mouse.left_pressed = 0;
    e->e_mouse.right_pressed = 0;
    e->e_mouse.middle_pressed = 0;

    e->t_mouse.x = 0;
    e->t_mouse.y = 0;
    e->t_mouse.click_x = 0;
    e->t_mouse.click_y = 0;
    e->t_mouse.left_pressed = 0;
    e->t_mouse.right_pressed = 0;
    e->t_mouse.middle_pressed = 0;

    e->s_mouse.x = 0;
    e->s_mouse.y = 0;
    e->s_mouse.click_x = 0;
    e->s_mouse.click_y = 0;
    e->s_mouse.left_pressed = 0;
    e->s_mouse.right_pressed = 0;
    e->s_mouse.middle_pressed = 0;

    e->settings.layer = 0;
    e->settings.collidable = 0;
    e->settings.render_layer_1 = 1;
    e->settings.render_layer_2 = 1;
    e->settings.render_layer_3 = 1;
    GINFO("Settings initialized");
}
