#include <SDL2/SDL.h>
#include "panel.h"
#include "../../defs.h"
#include "../../shared/sprite.h"
#include "../../shared/logger.h"

SpritePanel* init_panel(SpriteMap* sprite_map) {
    SpritePanel* panel = malloc(sizeof(SpritePanel));
    if (panel == NULL) {
        GFATAL("Failed to allocate memory for panel");
        return NULL;
    }

    panel->rect = (SDL_Rect){ 0, 0, PANEL_WIDTH, SCREEN_HEIGHT + 15};
    panel->sprite_count = 0;
    panel->selected_sprite = NULL;
    panel->sprites = malloc(sizeof(SpriteCoord*) * sprite_map->count);
    if (panel->sprites == NULL) {
        GFATAL("Failed to allocate memory for panel sprites");
        free(panel);
        return NULL;
    }

    GDEBUG("count: %d", sprite_map->count);

    int xOffset = 5;
    int yOffset = 5;

    for (int i = 0; i < sprite_map->count; i++) {
        if (panel->sprite_count >= PANEL_COLS * PANEL_ROWS) break;

        Sprite* sprite = &sprite_map->sprites[i];
        
        // Check if the sprite name ends with _f followed by a number
        char* last_underscore = strrchr(sprite->name, '_');
        if (last_underscore == NULL || *(last_underscore + 1) != 'f' || !isdigit(*(last_underscore + 2))) {
            panel->sprites[panel->sprite_count] = malloc(sizeof(SpriteCoord));
            if (panel->sprites[panel->sprite_count] == NULL) {
                GERROR("Failed to allocate memory for SpriteCoord");
                // TODO: Free previously allocated memory
                return NULL;
            }

            panel->sprites[panel->sprite_count]->sprite = sprite;
            panel->sprites[panel->sprite_count]->x = xOffset;
            panel->sprites[panel->sprite_count]->y = yOffset;
            panel->sprite_count++;
            GINFO("Added sprite: %s at %d, %d, new count: %d", sprite->name, xOffset, yOffset, panel->sprite_count);

            xOffset += SPRITE_WIDTH + PANEL_PADDING;

            if (xOffset >= PANEL_WIDTH) {
                xOffset = 5;
                yOffset = yOffset + SPRITE_HEIGHT + PANEL_PADDING;
            }
        }

    }

    return panel;
}

Sprite* find_sprite_panel(SpritePanel* panel, int x, int y) {
    for (int i = 0; i < panel->sprite_count; i++) {
        if (panel->sprites[i]->x == x && panel->sprites[i]->y == y) {
            return panel->sprites[i]->sprite;
        }
    }

    return NULL;
}

int compare_sprites(const void* a, const void* b) {
    const SpriteCoord* sprite_a = *(const SpriteCoord**)a;
    const SpriteCoord* sprite_b = *(const SpriteCoord**)b;
    
    int area_a = sprite_a->sprite->rect.w * sprite_a->sprite->rect.h;
    int area_b = sprite_b->sprite->rect.w * sprite_b->sprite->rect.h;
    
    return area_a - area_b;
}

void render_panel(SDL_Renderer* renderer,
                  SDL_Texture* spritesheet,
                  SpritePanel* panel,
                  Sprite* selected_sprite) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &panel->rect);

    qsort(panel->sprites, panel->sprite_count, sizeof(SpriteCoord*), compare_sprites);

    int current_y = panel->rect.y;

    // Render sprites in the sorted order, stacking them vertically
    for (int i = 0; i < panel->sprite_count; i++) {
        SpriteCoord* coord = panel->sprites[i];
        Sprite* sprite = coord->sprite;
        
        SDL_Rect dest = { 
            panel->rect.x, 
            current_y, 
            sprite->rect.w, 
            sprite->rect.h 
        };
        
        SDL_RenderCopy(renderer, spritesheet, &sprite->rect, &dest);
        
        if (sprite == panel->selected_sprite) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &dest);
        }

        current_y += sprite->rect.h + 5; // 5 pixel gap between sprites
    }
}

void free_panel(SpritePanel* panel) {
    for (int i = 0; i < panel->sprite_count; i++) {
        free(panel->sprites[i]);
    }

    free(panel->sprites);
    free(panel);
}
