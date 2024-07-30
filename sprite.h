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

extern SpriteMap sprite_map;
extern AnimatedSpriteMap animation_map;
extern SDL_Texture* spritesheet;

int init_sprites(SDL_Renderer* renderer,
                 const char* spritesheet_path,
                 const char* spritedata_path);

AnimatedSprite* create_animation(const char* name, float frame_duration);

int load_sprites(const char* filename);
void load_animations();

Sprite* find_sprite(SpriteMap* sprite_map, const char* name);
AnimatedSprite* find_animation(const char* name);

void render_sprite(SDL_Renderer* renderer,
                   const char* name,
                   int x,
                   int y);

void render_animation(SDL_Renderer* renderer,
                      AnimatedSprite* anim,
                      int x,
                      int y);

void update_animation(AnimatedSprite* anim, float delta_time);

void destroy_animation(AnimatedSprite* anim);

void cleanup_spritesheet(SDL_Texture* spritesheet);

#endif
