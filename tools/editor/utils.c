#include "utils.h"
#include "defs.h"

// djb2 hash
// credit: http://www.cse.yorku.ca/~oz/hash.html
unsigned long
hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

void screen_to_tilesheet(int w,
                         int h,
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
    *tile_x = (*tile_x < 0) ? 0 : (*tile_x >= w) ? w - 1 : *tile_x;
    *tile_y = ((screen_y - EDITOR_TOOL_HEIGHT) - zoom_y) / scale;
}

void tilesheet_to_screen(int zoom_x,
                         int zoom_y,
                         float scale,
                         int tile_x,
                         int tile_y,
                         int* screen_x,
                         int* screen_y) {
    *screen_x = (tile_x * scale) + zoom_x;
    *screen_y = (tile_y * scale) + zoom_y + EDITOR_TOOL_HEIGHT;
}

int snap_to_grid(int coord, float scale) {
    float scaled_tile = TILE_SIZE * scale;

    return (int)(roundf(coord / scaled_tile) * scaled_tile);
}

void apply_zoom(ZoomState* z, float new_scale, int m_x, int m_y) {
    float old_scale = z->scale;
    z->scale = new_scale;

    float world_x = (m_x - z->offset_x) / old_scale;
    float world_y = ((m_y - EDITOR_TOOL_HEIGHT) - z->offset_y) / old_scale;

    z->offset_x = snap_to_grid(m_x - (world_x * new_scale), new_scale);
    z->offset_y = snap_to_grid((m_y - EDITOR_TOOL_HEIGHT) - (world_y * new_scale), new_scale);

    if (fabsf(new_scale - 1.0f) < 0.001f) {
        z->offset_x = 0;
        z->offset_y = 0;
    }
}

SDL_bool is_window_focused(SDL_Window* window) {
    uint32_t flags = SDL_GetWindowFlags(window);
    return (flags & SDL_WINDOW_INPUT_FOCUS) != 0;
}

