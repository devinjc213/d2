#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>

#define MAX_SPRITES 370
#define MAX_FRAMES 10

#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 16

#define MAX_NAME_LENGTH 50

#define MAX_ANIMATIONS 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    SDL_Rect rect;
} Sprite;

typedef struct {
    Sprite sprites[MAX_SPRITES];
    int count;
} SpriteMap;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int start_index;
    int frame_count;
    int current_frame;
    float frame_duration;
    float time_accumulated;
} AnimatedSprite;

typedef struct {
    AnimatedSprite sprites[MAX_SPRITES];
    int count;
} AnimatedSpriteMap;

AnimatedSpriteMap* getSprites();

int load_sprites(const char* filename, SpriteMap* sprite_map);
void load_animations(SpriteMap* sprite_map, AnimatedSpriteMap* animation_map);

Sprite* find_sprite(SpriteMap* sprite_map, const char* name);

void render_sprite(SDL_Renderer* renderer, SpriteMap* sprite_map, SDL_Texture* spritesheet, const char* name, int x, int y);

int init_sprites(SDL_Renderer* renderer,
                 SDL_Texture** spritesheet,
                 SpriteMap* sprite_map,
                 const char* spritesheet_path,
                 const char* spritedata_path);

AnimatedSprite* create_animation(const char* name, float frame_duration);


AnimatedSprite* find_animation(AnimatedSpriteMap* animation_map, const char* name);

void add_frame_to_animation(AnimatedSprite* anim, Sprite* sprite);

void update_animation(AnimatedSprite* anim, float delta_time);

void render_animation(SDL_Renderer* renderer,
                      SpriteMap* sprite_map,
                      SDL_Texture* spritesheet,
                      AnimatedSprite* anim,
                      int x,
                      int y);

void destroy_animation(AnimatedSprite* anim);

void cleanup_spritesheet(SDL_Texture* spritesheet);


#endif
