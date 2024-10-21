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
#include "editor.h"

struct nk_context *nk_ctx;

void cleanup_editor(Editor* editor);
void render_grid(SDL_Renderer* renderer, int offset_x, int offset_y, float scale);
SDL_bool is_window_focused(SDL_Window* window);
int snap_to_grid(int coord, float scale);

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) != (IMG_INIT_PNG)) {
        SDL_Quit();
        return 1;
    }

    Editor* editor = malloc(sizeof(Editor));
    init_editor(editor);

    GINFO("Initilizing nuklear...");
    nk_ctx = nk_sdl_init(editor->s_win, editor->s_render);
    struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    nk_sdl_font_stash_end();
    GINFO("nuklear initialized");

    while (!editor->quit) {
        SDL_Event e;
        nk_input_begin(nk_ctx);

        while (SDL_PollEvent(&e) != 0) {
            if (e.window.windowID == SDL_GetWindowID(editor->s_win)) {
                nk_sdl_handle_event(&e);
            }

            switch (e.type) {
                case SDL_QUIT:
                    editor->quit = 1;
                    break;

                case SDL_MOUSEWHEEL:
                    if (is_window_focused(editor->t_win)) {
                        GINFO("pre-scale: %f", editor->t_zoom.scale);
                        float new_scale = editor->t_zoom.scale;
                        if (e.wheel.y > 0 && new_scale < 2.0f) {
                            new_scale += 0.2f;
                        } else if (e.wheel.y < 0 && new_scale > 1.0f) {
                            new_scale -= 0.2f;
                        }
                        
                        if (new_scale != editor->t_zoom.scale) {
                            int x, y;
                            SDL_GetMouseState(&x, &y);
                            GINFO("mouse x: %d, y: %d", x, y);
                            apply_zoom(&editor->t_zoom, new_scale, x, y);
                        }
                    }

                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        editor->mouse_held_down = 1;
                    } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                        editor->t_mm_pressed = 1;
                    }

                    if (is_window_focused(editor->t_win)) {
                        SDL_GetMouseState(&editor->t_select_m_x, &editor->t_select_m_y);
                        GINFO("raw start x: %d, y: %d", editor->t_select_m_x, editor->t_select_m_y);
                        int start_tile_x, start_tile_y;
                        i_screen_to_tilesheet(editor, 
                                            editor->t_zoom.offset_x, 
                                            editor->t_zoom.offset_y, 
                                            editor->t_zoom.scale, 
                                            editor->t_select_m_x, 
                                            editor->t_select_m_y, 
                                            &start_tile_x, 
                                            &start_tile_y);

                        editor->t_select_m_x = start_tile_x;
                        editor->t_select_m_y = start_tile_y;

                        GINFO("translated start x: %d, y: %d", start_tile_x, start_tile_y);
                    }

                    if (is_window_focused(editor->e_win) && editor->select_buf->active_selection == 1) {
                        SDL_GetMouseState(&editor->e_cur_m_x, &editor->e_cur_m_y);

                        SDL_Rect dest = {
                            snap_to_grid(editor->e_cur_m_x, editor->e_zoom.scale),
                            snap_to_grid(editor->e_cur_m_y, editor->e_zoom.scale),
                            editor->select_buf->rect.w,
                            editor->select_buf->rect.h
                        };

                        RenderTile tile = {
                            .layer = editor->settings.layer,
                            .src = editor->select_buf->rect,
                            .dest =  dest,
                            .tilesheet = editor->cur_sheet_index
                        };

                        add_render_tile(&editor->tile_map->render_layers[editor->settings.layer], tile); 
                    }

                    break;

                case SDL_MOUSEMOTION:
                    if (is_window_focused(editor->t_win)) {
                        int cur_x, cur_y, translate_x, translate_y;
                        SDL_GetMouseState(&cur_x, &cur_y);

                        i_screen_to_tilesheet(editor, 
                                            editor->t_zoom.offset_x, 
                                            editor->t_zoom.offset_y, 
                                            editor->t_zoom.scale, 
                                            cur_x,
                                            cur_y,
                                            &translate_x, 
                                            &translate_y);
                        editor->t_cur_m_x = translate_x;
                        editor->t_cur_m_y = translate_y;
                    }

                    if (is_window_focused(editor->e_win)) {
                        SDL_GetMouseState(&editor->e_cur_m_x, &editor->e_cur_m_y);
                    }

                    break;

                case SDL_MOUSEBUTTONUP:
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        editor->mouse_held_down = 0;
                    } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                        editor->t_mm_pressed = 0;
                    }

                    if (is_window_focused(editor->t_win)) {
                        int cur_x, cur_y, translate_x, translate_y;
                        SDL_GetMouseState(&cur_x, &cur_y);
                        i_screen_to_tilesheet(editor, 
                                            editor->t_zoom.offset_x, 
                                            editor->t_zoom.offset_y, 
                                            editor->t_zoom.scale, 
                                            cur_x,
                                            cur_y,
                                            &translate_x, 
                                            &translate_y);
                        editor->t_cur_m_x = translate_x;
                        editor->t_cur_m_y = translate_y;

                        int snapBX = snap_to_grid(editor->t_select_m_x, 1.0f);
                        int snapBY = snap_to_grid(editor->t_select_m_y, 1.0f);
                        int snapCX = snap_to_grid(editor->t_cur_m_x, 1.0f);
                        int snapCY = snap_to_grid(editor->t_cur_m_y, 1.0f);
                        SDL_Rect selection = {snapBX, snapBY, snapCX - snapBX, snapCY - snapBY};
        
                        editor->select_buf->rect = selection;
                        editor->select_buf->tilesheet = editor->cur_sheet_index;
                        editor->select_buf->active_selection = 1;
                        editor->t_select_m_x = 0;
                        editor->t_select_m_y = 0;
                    }

                    break;  

                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE && editor->select_buf->active_selection == 1) {
                        editor->select_buf->active_selection = 0;
                        editor->select_buf->rect.x = 0;
                        editor->select_buf->rect.x = 0;
                        editor->select_buf->rect.x = 0;
                        editor->select_buf->rect.x = 0;
                    }

                    if ((e.key.keysym.sym == SDLK_RIGHT || e.key.keysym.sym == SDLK_LEFT) && is_window_focused(editor->t_win)) {
                        switch (e.key.keysym.sym) {
                            case SDLK_RIGHT:
                                editor->cur_sheet_index = (editor->cur_sheet_index + 1) % editor->total_sheets;
                                break;
                            case SDLK_LEFT:
                                editor->cur_sheet_index = (editor->cur_sheet_index - 1 + editor->total_sheets) % editor->total_sheets;
                                break;
                        }
                        editor->cur_t_sheet = get_texture(editor->t_cache, editor->t_render, editor->cur_sheet_index);
                        editor->cur_e_sheet = get_texture(editor->e_cache, editor->e_render, editor->cur_sheet_index);

                        SDL_QueryTexture(editor->cur_t_sheet, NULL, NULL, &editor->t_w, &editor->t_h);

                        SDL_SetWindowSize(editor->t_win, editor->t_w, editor->t_h);
                    }
            }
        }
        nk_input_end(nk_ctx);

        SDL_SetRenderDrawColor(editor->e_render, 100, 100, 100, 255);
        SDL_SetRenderDrawColor(editor->t_render, 100, 100, 100, 255);
        SDL_SetRenderDrawColor(editor->s_render, 100, 100, 100, 255);

        if (nk_begin(nk_ctx,
                     "Settings",
                     nk_rect(0, 0, 300, 768),
                     NK_WINDOW_BORDER|NK_WINDOW_TITLE)
        ) {
            nk_layout_row_dynamic(nk_ctx, 30, 3);
            if (nk_option_label(nk_ctx, "Layer 1", editor->settings.layer == 0)) editor->settings.layer = 0;
            if (nk_option_label(nk_ctx, "Layer 2", editor->settings.layer == 1)) editor->settings.layer = 1;
            if (nk_option_label(nk_ctx, "Layer 3", editor->settings.layer == 2)) editor->settings.layer = 2;

            nk_layout_row_dynamic(nk_ctx, 30, 1);
            nk_bool check = editor->settings.collidable;
            if(nk_checkbox_label(nk_ctx, "Collidable", &check)) editor->settings.collidable = check;
        }
        nk_end(nk_ctx);

        SDL_RenderClear(editor->e_render);
        SDL_RenderClear(editor->t_render);
        SDL_RenderClear(editor->s_render);

        SDL_RenderSetScale(editor->t_render, 1.0f, 1.0f);

        render_layer(&editor->tile_map->render_layers[0], editor->e_render, editor->e_cache);

        render_grid(editor->e_render, editor->e_zoom.offset_x, editor->e_zoom.offset_y, editor->e_zoom.scale);

        SDL_Rect src_rect = {0, 0, editor->t_w, editor->t_h};
        SDL_Rect t_zoom = {
            editor->t_zoom.offset_x,
            editor->t_zoom.offset_y,
            editor->t_w * editor->t_zoom.scale,
            editor->t_h * editor->t_zoom.scale
        };
        SDL_RenderCopy(editor->t_render, editor->cur_t_sheet, &src_rect, &t_zoom);

        // draw active selection while selecting
        if (is_window_focused(editor->t_win) && editor->mouse_held_down == 1) {
            GINFO("mouse held down");
            int scaled_cur_x, scaled_cur_y, scaled_select_x, scaled_select_y;
            r_tilesheet_to_screen(editor,
                                  editor->t_zoom.offset_x,
                                  editor->t_zoom.offset_y,
                                  editor->t_zoom.scale,
                                  editor->t_cur_m_x,
                                  editor->t_cur_m_y,
                                  &scaled_cur_x,
                                  &scaled_cur_y);
            r_tilesheet_to_screen(editor,
                                  editor->t_zoom.offset_x,
                                  editor->t_zoom.offset_y,
                                  editor->t_zoom.scale,
                                  editor->t_select_m_x,
                                  editor->t_select_m_y,
                                  &scaled_select_x,
                                  &scaled_select_y);
            int snapBX = snap_to_grid(scaled_select_x, editor->t_zoom.scale);
            int snapBY = snap_to_grid(scaled_select_y, editor->t_zoom.scale);
            int snapCX = snap_to_grid(scaled_cur_x, editor->t_zoom.scale);
            int snapCY = snap_to_grid(scaled_cur_y, editor->t_zoom.scale);
            SDL_SetRenderDrawBlendMode(editor->t_render, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(editor->t_render, 0, 0, 255, 100);
            SDL_Rect selection = {snapBX, snapBY, snapCX - snapBX, snapCY - snapBY};
            SDL_RenderFillRect(editor->t_render, &selection);
            SDL_RenderDrawRect(editor->t_render, &selection);
        }

        if (editor->select_buf->active_selection) {
            SDL_SetRenderDrawBlendMode(editor->t_render, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(editor->t_render, 0, 0, 255, 100);
            int scaled_x, scaled_y;
            r_tilesheet_to_screen(editor,
                                  editor->t_zoom.offset_x,
                                  editor->t_zoom.offset_y,
                                  editor->t_zoom.scale,
                                  editor->select_buf->rect.x,
                                  editor->select_buf->rect.y,
                                  &scaled_x,
                                  &scaled_y);

            SDL_Rect selection = {
                scaled_x,
                scaled_y,
                editor->select_buf->rect.w * editor->t_zoom.scale,
                editor->select_buf->rect.h * editor->t_zoom.scale
            };
            SDL_RenderFillRect(editor->t_render, &selection);
            SDL_RenderDrawRect(editor->t_render, &selection);
        }

        render_grid(editor->t_render, editor->t_zoom.offset_x, editor->t_zoom.offset_y, editor->t_zoom.scale);
        nk_sdl_render(NK_ANTI_ALIASING_ON);

        if (is_window_focused(editor->e_win)) {
            int snapX = snap_to_grid(editor->e_cur_m_x, editor->e_zoom.scale);
            int snapY = snap_to_grid(editor->e_cur_m_y, editor->e_zoom.scale);

            SDL_Rect dest = {snapX, snapY, editor->select_buf->rect.w, editor->select_buf->rect.h};
            if (editor->cur_e_sheet != NULL && editor->select_buf->active_selection) {
                SDL_SetRenderDrawBlendMode(editor->e_render, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(editor->e_render, 0, 0, 255, 100);
                SDL_RenderCopy(editor->e_render, editor->cur_e_sheet, &editor->select_buf->rect, &dest);
                SDL_RenderFillRect(editor->e_render, &dest);
                SDL_RenderDrawRect(editor->e_render, &dest);
            }
        }


        SDL_RenderPresent(editor->e_render);
        SDL_RenderPresent(editor->t_render);
        SDL_RenderPresent(editor->s_render);
    }

    destroy_texture_cache(editor->e_cache);
    destroy_texture_cache(editor->t_cache);
    cleanup_editor(editor);
    return 0;
}

void render_grid(SDL_Renderer* renderer, int offset_x, int offset_y, float scale) {
    SDL_SetRenderDrawColor(renderer, 83, 83, 83, 155);

    float tile_size_scaled = TILE_SIZE * scale;

    float start_x = -fmodf(offset_x, tile_size_scaled);
    if (start_x > 0) start_x -= tile_size_scaled;

    float start_y = -fmodf(offset_y, tile_size_scaled);
    if (start_y > 0) start_y -= tile_size_scaled;

    // Draw vertical grid lines
    for (float x = start_x; x < SCREEN_WIDTH; x += tile_size_scaled) {
        SDL_RenderDrawLine(renderer, (int)x, 0, (int)x, SCREEN_HEIGHT);
    }

    // Draw horizontal grid lines
    for (float y = start_y; y < SCREEN_HEIGHT; y += tile_size_scaled) {
        SDL_RenderDrawLine(renderer, 0, (int)y, SCREEN_WIDTH, (int)y);
    }
}

int snap_to_grid(int coord, float scale) {
    float scaled_tile = TILE_SIZE * scale;

    return (int)(floorf(coord / scaled_tile) * scaled_tile);
}

SDL_bool is_window_focused(SDL_Window* window) {
    uint32_t flags = SDL_GetWindowFlags(window);
    return (flags & SDL_WINDOW_INPUT_FOCUS) != 0;
}

void cleanup_editor(Editor* editor) {
    SDL_DestroyRenderer(editor->e_render);
    SDL_DestroyRenderer(editor->t_render);
    SDL_DestroyRenderer(editor->s_render);
    SDL_DestroyWindow(editor->e_win);
    SDL_DestroyWindow(editor->t_win);
    SDL_DestroyWindow(editor->s_win);

    IMG_Quit();
    SDL_Quit();
}
