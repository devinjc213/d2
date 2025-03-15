#ifndef VECTOR2_H
#define VECTOR2_H

typedef struct {
  float x;
  float y;

  float magnitude;

  float mass;
  float friction;
} Vector2;

void vec2_add(Vector2* vec, Vector2 add);
void vec2_sub(Vector2* vec, Vector2 sub);
void vec2_scale(Vector2* vec, float scalar);
float vec2_magnitude(Vector2* vec);

void vec2_up(Vector2* vec, float amount);
void vec2_down(Vector2* vec, float amount);
void vec2_left(Vector2* vec, float amount);
void vec2_right(Vector2* vec, float amount);

#endif
