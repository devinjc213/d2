#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "sprite.h"
#include "logger.h"

int load_sprites(const char* filename, SpriteMap* sprite_map) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        GFATAL("Failed to read sprite data file: %s\n", filename);
        return 1;
    }

    char name[MAX_NAME_LENGTH];
    int x, y, w, h;

    while(fscanf(file, "%s %d %d %d %d", name, &x, &y, &w, &h) == 5) {
        if (sprite_map->count >= MAX_SPRITES) {
            GWARN("Max sprites reached. Ignoring remaining.\n");
            break;
        }

        strncpy(sprite_map->sprites[sprite_map->count].name, name, MAX_NAME_LENGTH -1);
        sprite_map->sprites[sprite_map->count].rect = (SDL_Rect){x, y, w, h};
        sprite_map->count++;
    }

    fclose(file);
    return 0;
}

void load_animations(SpriteMap* sprite_map, AnimatedSpriteMap* animation_map) {
    int current_start_index = 0;
    char prev_base_name[MAX_NAME_LENGTH] = "";
    animation_map->count = 0;

    for (int i = 0; i < sprite_map->count; i++) {
        Sprite* sprite = &sprite_map->sprites[i];
        
        // Check if the sprite name ends with _f followed by a number
        char* last_underscore = strrchr(sprite->name, '_');
        if (last_underscore == NULL || *(last_underscore + 1) != 'f' || !isdigit(*(last_underscore + 2))) {
            continue;  // Skip this sprite as it doesn't match the _f[number] pattern
        }

        // Extract base name (everything before the last underscore)
        char base_name[MAX_NAME_LENGTH];
        size_t base_name_length = last_underscore - sprite->name;
        strncpy(base_name, sprite->name, base_name_length);
        base_name[base_name_length] = '\0';

        // If this is a new base name, start a new animation
        if (strcmp(base_name, prev_base_name) != 0) {
            if (animation_map->count > 0) {
                // Set the frame count for the previous animation
                animation_map->sprites[animation_map->count - 1].frame_count = i - current_start_index;
            }
            
            if (animation_map->count >= MAX_ANIMATIONS) {
                printf("Warning: Maximum number of animations reached.\n");
                break;
            }
            
            AnimatedSprite* new_anim = &animation_map->sprites[animation_map->count++];
            strncpy(new_anim->name, base_name, MAX_NAME_LENGTH - 1);
            new_anim->start_index = i;
            new_anim->current_frame = 0;
            new_anim->frame_duration = 0.1f;  // Default frame duration, adjust as needed
            new_anim->time_accumulated = 0.0f;
            
            current_start_index = i;
            strncpy(prev_base_name, base_name, MAX_NAME_LENGTH - 1);
        }
    }
    
    // Set the frame count for the last animation
    if (animation_map->count > 0) {
        animation_map->sprites[animation_map->count - 1].frame_count = sprite_map->count - current_start_index;
    }
}

Sprite* find_sprite(SpriteMap* sprite_map, const char* name) {
    for (int i = 0; i < sprite_map->count; i++) {
        if (strcmp(sprite_map->sprites[i].name, name) == 0) {
            return &sprite_map->sprites[i];
        }
    }
    
    return NULL;
}

AnimatedSprite* find_animation(AnimatedSpriteMap* animation_map, const char* name) {
    for (int i = 0; i < animation_map->count; i++) {
        if (strcmp(animation_map->sprites[i].name, name) == 0) {
            return &animation_map->sprites[i];
        }
    }
    return NULL;
}

void render_sprite(SDL_Renderer* renderer,
                   SpriteMap* sprite_map,
                   SDL_Texture* spritesheet,
                   const char* name,
                   int x,
                   int y) {
    Sprite* sprite = find_sprite(sprite_map, name);
    if (sprite == NULL) {
        GERROR("Failed to load sprite for rendering: %s", name);
    }

    SDL_Rect dest = {x, y, sprite->rect.w, sprite->rect.h};
    SDL_RenderCopy(renderer, spritesheet, &sprite->rect, &dest);
}

int init_sprites(SDL_Renderer* renderer,
                 SDL_Texture** spritesheet,
                 SpriteMap* sprite_map,
                 const char* spritesheet_path,
                 const char* spritedata_path) {
    SDL_Surface* surface = IMG_Load(spritesheet_path);
    if (surface == NULL) {
        GFATAL("Failed to load spritesheet: %s\n", IMG_GetError());
        return 1;
    }

    *spritesheet = SDL_CreateTextureFromSurface(renderer, surface);
    if(spritesheet == NULL) {
        GFATAL("Failed to create spritesheet texture: %s\n", SDL_GetError());
        return 1;
    }

    load_sprites(spritedata_path, sprite_map);

    return 0;
}

void cleanup_spritesheet(SDL_Texture* spritesheet) {
    SDL_DestroyTexture(spritesheet);
}

AnimatedSprite* create_animation(const char* name, float frame_duration) {
    AnimatedSprite* anim = (AnimatedSprite*)malloc(sizeof(AnimatedSprite));
    if (anim == NULL) return NULL;
    
    strncpy(anim->name, name, MAX_NAME_LENGTH - 1);
    anim->frame_count = 0;
    anim->current_frame = 0;
    anim->frame_duration = frame_duration;
    anim->time_accumulated = 0.0f;
    
    return anim;
}

// Update animation based on elapsed time
void update_animation(AnimatedSprite* anim, float delta_time) {
    anim->time_accumulated += delta_time;
    if (anim->time_accumulated >= anim->frame_duration) {
        anim->current_frame = (anim->current_frame + 1) % anim->frame_count;
        anim->time_accumulated -= anim->frame_duration;
    }
}

// Render the current frame of an animation
void render_animation(SDL_Renderer* renderer,
                      SpriteMap* sprite_map,
                      SDL_Texture* spritesheet,
                      AnimatedSprite* anim,
                      int x,
                      int y) {
    if (anim->frame_count > 0) {
        int sprite_index = anim->start_index + anim->current_frame;
        Sprite* current_sprite = &sprite_map->sprites[sprite_index];
        SDL_Rect dest = {x, y, current_sprite->rect.w, current_sprite->rect.h};
        SDL_RenderCopy(renderer, spritesheet, &current_sprite->rect, &dest);
    }
}

// Clean up animation
void destroy_animation(AnimatedSprite* anim) {
    free(anim);
}
