#include "input.h"
#include "../../shared/logger.h"
#include "../../shared/nuklear.h"
#include "../../shared/nuklear_sdl_renderer.h"
#include "editor.h"

static void handle_any_input(SDL_Event *e, Editor *editor);
static void handle_editor_input(SDL_Event *e, Editor *editor);
static void handle_tilesheet_input(SDL_Event *e, Editor *editor);
static void handle_mousewheel(SDL_Event *e, ZoomState *z);

void handle_input(SDL_Event *e, Editor *editor) {
  if (e->type == SDL_QUIT) {
    editor->quit = 1;
  }

  handle_any_input(e, editor);

  if (is_window_focused(editor->e_win)) {
    handle_editor_input(e, editor);
  } else if (is_window_focused(editor->t_win)) {
    handle_tilesheet_input(e, editor);
  } else if (is_window_focused(editor->s_win)) {
    handle_settings_input(e, editor);
  }
}

static void handle_any_input(SDL_Event *e, Editor *editor) {
  switch (e->type) {
  case SDL_KEYDOWN:
    if (e->key.keysym.sym == SDLK_ESCAPE &&
        editor->select_buf.active_selection == 1) {
      editor->select_buf.active_selection = 0;
      editor->select_buf.rect.x = 0;
      editor->select_buf.rect.x = 0;
      editor->select_buf.rect.x = 0;
      editor->select_buf.rect.x = 0;
    }

    break;
  }
}

static void handle_editor_input(SDL_Event *e, Editor *editor) {
  switch (e->type) {
  case SDL_MOUSEWHEEL:
    handle_mousewheel(e, &editor->e_zoom);
    break;

  case SDL_MOUSEBUTTONDOWN: {
    int x, y;
    SDL_GetMouseState(&editor->e_mouse.click_x, &editor->e_mouse.click_y);

    screen_to_world(editor->e_w, editor->e_h, editor->e_zoom.offset_x,
                    editor->e_zoom.offset_y, editor->e_zoom.scale,
                    editor->e_mouse.click_x, editor->e_mouse.click_y, &x, &y);

    editor->e_mouse.click_x = x;
    editor->e_mouse.click_y = y;

    SDL_Rect dest = {
        snap_to_grid(editor->e_mouse.click_x, editor->e_zoom.scale),
        snap_to_grid(editor->e_mouse.click_y, editor->e_zoom.scale),
        editor->select_buf.rect.w, editor->select_buf.rect.h};

    GINFO("dest rect: x: %d, y: %d, w: %d, h: %d", editor->e_mouse.click_x,
          editor->e_mouse.click_y, editor->select_buf.rect.w,
          editor->select_buf.rect.h);

    RenderTile tile = {.layer = editor->settings.layer,
                       .src = editor->select_buf.rect,
                       .dest = dest,
                       .tilesheet =
                           editor->d_asset_dir.paths[editor->cur_sheet_index]};

    add_render_tile(&editor->tile_map->render_layers[editor->settings.layer],
                    tile);

    break;
  }

  case SDL_MOUSEMOTION:
    SDL_GetMouseState(&editor->e_mouse.x, &editor->e_mouse.y);

    break;

  case SDL_MOUSEBUTTONUP:
    if (e->button.button == SDL_BUTTON_LEFT) {
      editor->e_mouse.left_pressed = 0;
    } else if (e->button.button == SDL_BUTTON_MIDDLE) {
      editor->e_mouse.middle_pressed = 0;
    } else if (e->button.button == SDL_BUTTON_RIGHT) {
      editor->e_mouse.right_pressed = 0;
    }

    break;
  }
}

static void handle_tilesheet_input(SDL_Event *e, Editor *editor) {
  switch (e->type) {
  case SDL_MOUSEWHEEL:
    handle_mousewheel(e, &editor->t_zoom);
    break;

  case SDL_MOUSEBUTTONDOWN:
    if (e->button.button == SDL_BUTTON_LEFT) {
      editor->t_mouse.left_pressed = 1;
    } else if (e->button.button == SDL_BUTTON_MIDDLE) {
      editor->t_mouse.middle_pressed = 1;
    } else if (e->button.button == SDL_BUTTON_RIGHT) {
      editor->t_mouse.right_pressed = 1;
    }

    SDL_GetMouseState(&editor->t_mouse.click_x, &editor->t_mouse.click_y);
    int start_tile_x, start_tile_y;
    screen_to_world(editor->t_w, editor->t_h, editor->t_zoom.offset_x,
                    editor->t_zoom.offset_y, editor->t_zoom.scale,
                    editor->t_mouse.click_x, editor->t_mouse.click_y,
                    &start_tile_x, &start_tile_y);

    editor->t_mouse.click_x = start_tile_x;
    editor->t_mouse.click_y = start_tile_y;

    break;

  case SDL_MOUSEMOTION: {
    int cur_x, cur_y, translate_x, translate_y;
    SDL_GetMouseState(&cur_x, &cur_y);

    screen_to_world(editor->t_w, editor->t_h, editor->t_zoom.offset_x,
                    editor->t_zoom.offset_y, editor->t_zoom.scale, cur_x, cur_y,
                    &translate_x, &translate_y);
    editor->t_mouse.x = translate_x;
    editor->t_mouse.y = translate_y;

    break;
  }

  case SDL_MOUSEBUTTONUP: {
    if (e->button.button == SDL_BUTTON_LEFT) {
      editor->t_mouse.left_pressed = 0;
    } else if (e->button.button == SDL_BUTTON_MIDDLE) {
      editor->t_mouse.middle_pressed = 0;
    } else if (e->button.button == SDL_BUTTON_RIGHT) {
      editor->t_mouse.right_pressed = 0;
    }

    int cur_x, cur_y, translate_x, translate_y;
    SDL_GetMouseState(&cur_x, &cur_y);
    screen_to_world(editor->t_w, editor->t_h, editor->t_zoom.offset_x,
                    editor->t_zoom.offset_y, editor->t_zoom.scale, cur_x, cur_y,
                    &translate_x, &translate_y);
    editor->t_mouse.x = translate_x;
    editor->t_mouse.y = translate_y;

    int snapBX = snap_to_grid(editor->t_mouse.click_x, 1.0f);
    int snapBY = snap_to_grid(editor->t_mouse.click_y, 1.0f);
    int snapCX = snap_to_grid(editor->t_mouse.x, 1.0f);
    int snapCY = snap_to_grid(editor->t_mouse.y, 1.0f);
    SDL_Rect selection = {snapBX, snapBY, snapCX - snapBX, snapCY - snapBY};

    editor->select_buf.rect = selection;
    editor->select_buf.tilesheet =
        editor->d_asset_dir.paths[editor->cur_sheet_index];
    editor->select_buf.active_selection = 1;
    editor->t_mouse.click_x = 0;
    editor->t_mouse.click_y = 0;

    break;
  }

  case SDL_KEYDOWN:
    if ((e->key.keysym.sym == SDLK_RIGHT || e->key.keysym.sym == SDLK_LEFT)) {
      switch (e->key.keysym.sym) {
      case SDLK_RIGHT:
        editor->cur_sheet_index =
            (editor->cur_sheet_index + 1) % editor->total_sheets;
        break;
      case SDLK_LEFT:
        editor->cur_sheet_index =
            (editor->cur_sheet_index - 1 + editor->total_sheets) %
            editor->total_sheets;
        break;
      }
      editor->cur_e_sheet = get_asset(
          editor->asset_map, editor->d_asset_dir.paths[editor->cur_sheet_index],
          R_EDITOR);
      editor->cur_t_sheet = get_asset(
          editor->asset_map, editor->d_asset_dir.paths[editor->cur_sheet_index],
          R_TILESHEETS);

      SDL_QueryTexture(editor->cur_t_sheet, NULL, NULL, &editor->t_w,
                       &editor->t_h);

      SDL_SetWindowSize(editor->t_win, editor->t_w, editor->t_h);
    }

    break;
  }
}

static void handle_mousewheel(SDL_Event *e, ZoomState *z) {
  float new_scale = z->scale;
  if (e->wheel.y > 0 && new_scale < 2.0f) {
    new_scale += 0.2f;
  } else if (e->wheel.y < 0 && new_scale > 1.0f) {
    new_scale -= 0.2f;
  }

  if (new_scale != z->scale) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    apply_zoom(z, new_scale, x, y);
  }
}
