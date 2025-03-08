#define NK_IMPLEMENTATION
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_BUTTON_TRIGGER_ON_RELEASE
#include "../../shared/nuklear.h"

#define NK_SDL_RENDERER_IMPLEMENTATION
#include "../../shared/nuklear_sdl_renderer.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "editor.h"
#include "utils.h"
#include "../../shared/logger.h"
#include "../../shared/asserts.h"
#include "../../engine/asset_manager.h"

static int init_sdl();
static void init_windows(Editor* e);
static void init_renderers(Editor* e);
static void init_textures(Editor* e, AssetMap* map);
static void init_tile_map(Editor* e);
static void init_settings(Editor* e);
static void init_nuklear(Editor* e);
static int load_asset_directory(Editor *e, const char* dir_path);

void init_editor(Editor* e) {
    if (init_sdl() != 0) {
        GFATAL("SDL failed to initialize.  Abandon all hope.");
        return;
    }

    init_windows(e);
    init_renderers(e);
    e->asset_map = malloc(sizeof(AssetMap));
    init_asset_map(e->asset_map);

    init_textures(e, e->asset_map);

    init_tile_map(e);
    init_settings(e);
    init_nuklear(e);
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

void render_editor_win(Editor* e) {
    SDL_SetRenderDrawColor(e->e_render, 100, 100, 100, 255);
    SDL_RenderClear(e->e_render);
    
    if (e->settings.render_layer_1) {
        render_layer(&e->tile_map->render_layers[0], e->e_render, 0, e->asset_map, &e->e_zoom);
    }

    if (e->settings.render_layer_2) {
        render_layer(&e->tile_map->render_layers[1], e->e_render, 0, e->asset_map, &e->e_zoom);
    }

    if (e->settings.render_layer_3) {
        render_layer(&e->tile_map->render_layers[2], e->e_render, 0, e->asset_map, &e->e_zoom);
    }

    render_grid(e->e_render, e->e_zoom.offset_x, e->e_zoom.offset_y, e->e_zoom.scale);

    if (e->select_buf.active_selection) {
        int snapX = snap_to_grid(e->e_mouse.x, e->e_zoom.scale);
        int snapY = snap_to_grid(e->e_mouse.y, e->e_zoom.scale);

        DASSERT(e->cur_e_sheet != NULL);
        SDL_Rect dest = {snapX, snapY, e->select_buf.rect.w * e->e_zoom.scale, (e->select_buf.rect.h * e->e_zoom.scale)};
        SDL_SetRenderDrawBlendMode(e->e_render, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(e->e_render, 0, 0, 255, 100);
        SDL_RenderCopy(e->e_render, e->cur_e_sheet, &e->select_buf.rect, &dest);
        SDL_RenderFillRect(e->e_render, &dest);
        SDL_RenderDrawRect(e->e_render, &dest);
    }

    nk_sdl_render(NK_ANTI_ALIASING_ON, e->e_nk_state);
    SDL_RenderPresent(e->e_render);

    nk_clear(&e->e_nk_state->ctx);
}

void render_tilesheet_win(Editor* e) {
    SDL_SetRenderDrawColor(e->t_render, 100, 100, 100, 255);
    SDL_RenderClear(e->t_render);
    SDL_RenderSetScale(e->t_render, 1.0f, 1.0f);

    SDL_Rect src_rect = {0, 0, e->t_w, e->t_h};
    SDL_Rect t_zoom = {
        e->t_zoom.offset_x,
        e->t_zoom.offset_y,
        e->t_w * e->t_zoom.scale,
        e->t_h * e->t_zoom.scale
    };
    SDL_RenderCopy(e->t_render, e->cur_t_sheet, &src_rect, &t_zoom);

    // draw active selection while selecting
    if (is_window_focused(e->t_win) && e->t_mouse.left_pressed == 1) {
        int scaled_cur_x, scaled_cur_y, scaled_select_x, scaled_select_y;
        world_to_screen(e->t_zoom.offset_x,
                            e->t_zoom.offset_y,
                            e->t_zoom.scale,
                            e->t_mouse.x,
                            e->t_mouse.y,
                            &scaled_cur_x,
                            &scaled_cur_y);
        world_to_screen(e->t_zoom.offset_x,
                            e->t_zoom.offset_y,
                            e->t_zoom.scale,
                            e->t_mouse.click_x,
                            e->t_mouse.click_y,
                            &scaled_select_x,
                            &scaled_select_y);
        int snapBX = snap_to_grid(scaled_select_x, e->t_zoom.scale);
        int snapBY = snap_to_grid(scaled_select_y, e->t_zoom.scale);
        int snapCX = snap_to_grid(scaled_cur_x, e->t_zoom.scale);
        int snapCY = snap_to_grid(scaled_cur_y, e->t_zoom.scale);
        SDL_SetRenderDrawBlendMode(e->t_render, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(e->t_render, 0, 0, 255, 100);
        SDL_Rect selection = {snapBX, snapBY, snapCX - snapBX, snapCY - snapBY};
        SDL_RenderFillRect(e->t_render, &selection);
        SDL_RenderDrawRect(e->t_render, &selection);
    }

    if (e->select_buf.active_selection) {
        SDL_SetRenderDrawBlendMode(e->t_render, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(e->t_render, 0, 0, 255, 100);
        int scaled_x, scaled_y;
        world_to_screen(e->t_zoom.offset_x,
                            e->t_zoom.offset_y,
                            e->t_zoom.scale,
                            e->select_buf.rect.x,
                            e->select_buf.rect.y,
                            &scaled_x,
                            &scaled_y);

        SDL_Rect selection = {
            scaled_x,
            scaled_y,
            e->select_buf.rect.w * e->t_zoom.scale,
            e->select_buf.rect.h * e->t_zoom.scale
        };
        SDL_RenderFillRect(e->t_render, &selection);
        SDL_RenderDrawRect(e->t_render, &selection);
    }

    render_grid(e->t_render, e->t_zoom.offset_x, e->t_zoom.offset_y, e->t_zoom.scale);
    
    SDL_RenderPresent(e->t_render);
}

void render_settings_win(Editor* e) {
    SDL_SetRenderDrawColor(e->s_render, 100, 100, 100, 255);
    SDL_RenderClear(e->s_render);


    if (nk_begin(&e->s_nk_state->ctx,
                    "Settings",
                    nk_rect(0, 0, 300, 768),
                    NK_WINDOW_BORDER|NK_WINDOW_TITLE)
    ) {
        nk_layout_row_dynamic(&e->s_nk_state->ctx, 30, 1);
        nk_label(&e->s_nk_state->ctx, "Paint Layers", NK_TEXT_LEFT);

        nk_layout_row_dynamic(&e->s_nk_state->ctx, 30, 3);
        if (nk_option_label(&e->s_nk_state->ctx, "Layer 1", e->settings.layer == 0)) e->settings.layer = 0;
        if (nk_option_label(&e->s_nk_state->ctx, "Layer 2", e->settings.layer == 1)) e->settings.layer = 1;
        if (nk_option_label(&e->s_nk_state->ctx, "Layer 3", e->settings.layer == 2)) e->settings.layer = 2;

        nk_spacer(&e->s_nk_state->ctx);

        nk_layout_row_dynamic(&e->s_nk_state->ctx, 30, 1);
        nk_label(&e->s_nk_state->ctx, "Tile Settings", NK_TEXT_LEFT);

        nk_layout_row_dynamic(&e->s_nk_state->ctx, 30, 1);
        nk_bool check = e->settings.collidable;
        if(nk_checkbox_label(&e->s_nk_state->ctx, "Collidable", &check)) e->settings.collidable = check;

        nk_spacer(&e->s_nk_state->ctx);

        nk_layout_row_dynamic(&e->s_nk_state->ctx, 30, 1);
        nk_label(&e->s_nk_state->ctx, "Render Layers", NK_TEXT_LEFT);

        nk_layout_row_dynamic(&e->s_nk_state->ctx, 30, 3);
        nk_bool render_1 = e->settings.render_layer_1;
        nk_bool render_2 = e->settings.render_layer_2;
        nk_bool render_3 = e->settings.render_layer_3;
        if(nk_checkbox_label(&e->s_nk_state->ctx, "Layer 1", &render_1)) e->settings.render_layer_1 = render_1;
        if(nk_checkbox_label(&e->s_nk_state->ctx, "Layer 2", &render_2)) e->settings.render_layer_2 = render_2;
        if(nk_checkbox_label(&e->s_nk_state->ctx, "Layer 3", &render_3)) e->settings.render_layer_3 = render_3;
    }
    nk_end(&e->s_nk_state->ctx);

    nk_sdl_render(NK_ANTI_ALIASING_ON, e->s_nk_state);
    SDL_RenderPresent(e->s_render);

    nk_clear(&e->s_nk_state->ctx);
}

void handle_settings_input(SDL_Event* e, Editor* editor) {
    nk_input_begin(&editor->s_nk_state->ctx);
    nk_sdl_handle_event(e, editor->s_nk_state);
    nk_input_end(&editor->s_nk_state->ctx);
}

void cleanup_editor(Editor* e) {
    SDL_DestroyRenderer(e->e_render);
    SDL_DestroyRenderer(e->t_render);
    SDL_DestroyRenderer(e->s_render);
    SDL_DestroyWindow(e->e_win);
    SDL_DestroyWindow(e->t_win);
    SDL_DestroyWindow(e->s_win);

    IMG_Quit();
    SDL_Quit();
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

static void init_textures(Editor* e, AssetMap* map) {
    GINFO("Initialzing textures...");
    e->cur_sheet_index = 0;
    e->total_sheets = load_asset_directory(e, "./bin/assets/Rogue/Rogue Adventure World 3.0.0/Tilesets (32x32)");
    if (e->total_sheets > 0) {
        SDL_Renderer* renderers[] = {e->e_render, e->t_render};

        for (int i = 0; i < e->total_sheets; i++) {
            SDL_Texture** textures = load_asset_multi_render(renderers, 2, e->d_asset_dir.paths[i]);
            add_asset(map, textures, 2, e->d_asset_dir.paths[i]);
        }

        e->cur_e_sheet = get_asset(map, e->d_asset_dir.paths[0], R_EDITOR);
        e->cur_t_sheet = get_asset(map, e->d_asset_dir.paths[0], R_TILESHEETS);
        SDL_QueryTexture(e->cur_t_sheet, NULL, NULL, &e->t_w, &e->t_h);
        SDL_SetWindowSize(e->t_win, e->t_w, e->t_h);

        GINFO("Textures initialized");
    } else {
        GWARN("Failed to load asset dir!");
    }
}

static void init_tile_map(Editor* e) {
    GINFO("Initializing tile map...");
    e->tile_map = malloc(sizeof(TileMap));
    e->tile_map = create_tilemap(64, 64, 3);
    GINFO("Tile map initialized");
}

static void init_settings(Editor* e) {
    // TODO use memcpy?
    GINFO("Initializing settings...");
    e->e_zoom.offset_x = 0;
    e->e_zoom.offset_y = 0;
    e->e_zoom.scale = 1.0f;
    e->t_zoom.offset_x = 0;
    e->t_zoom.offset_y = 0;
    e->t_zoom.scale = 1.0f;

    e->select_buf.rect.x = 0;
    e->select_buf.rect.y = 0;
    e->select_buf.rect.w = 0;
    e->select_buf.rect.h = 0;
    e->select_buf.tilesheet = 0;
    e->select_buf.active_selection = 0;

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

static void init_nuklear(Editor* e) {
    GINFO("Initilizing nuklear...");
    e->s_nk_state = malloc(sizeof(*e->s_nk_state));
    memset(e->s_nk_state, 0, sizeof(*e->s_nk_state));

    e->e_nk_state = malloc(sizeof(*e->e_nk_state));
    memset(e->e_nk_state, 0, sizeof(*e->e_nk_state));

    nk_sdl_font_stash_begin(&e->s_nk_atlas, e->s_nk_state);
    nk_sdl_init(e->s_win, e->s_render, e->s_nk_state);
    nk_sdl_font_stash_end(e->s_nk_state);

    nk_sdl_font_stash_begin(&e->e_nk_atlas, e->e_nk_state);
    nk_sdl_init(e->e_win, e->e_render, e->e_nk_state);
    nk_sdl_font_stash_end(e->e_nk_state);
    GINFO("nuklear initialized");
}

static int init_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) != (IMG_INIT_PNG)) {
        SDL_Quit();
        return 1;
    }

    return 0;
}

static int load_asset_directory(Editor *e, const char* dir_path) {
    DIR* dir;
    struct dirent* entry;

    for (int i = 0; i < MAX_ASSET_FILES; i++) {
        e->d_asset_dir.paths[i] = NULL;
    }
    e->d_asset_dir.count = 0;

    dir = opendir(dir_path);
    if (dir == NULL) {
        GERROR("Failed to open directory: %s\n", dir_path);
        return 0;
    }

    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_REG) {
            const char *ext = strrchr(entry->d_name, '.');

            if (ext && (
                strcmp(ext, ".png") == 0 ||
                strcmp(ext, ".bmp") == 0 || 
                strcmp(ext, ".jpg") == 0 ||
                strcmp(ext, ".jpeg") == 0
            )) {
                char full_path[MAX_FILENAME];
                snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

                if (e->d_asset_dir.count < MAX_ASSET_FILES) {
                    e->d_asset_dir.paths[e->d_asset_dir.count++] = strdup(full_path);
                } else {
                    GWARN("Maximum asset file limit reached. Skipping: %s\n", full_path);
                    break;
                }
            }
        }
    }
    
    GINFO("%d asset files scanned.", e->d_asset_dir.count);
    GINFO("%s", e->d_asset_dir.paths[0])

    return e->d_asset_dir.count;
}


