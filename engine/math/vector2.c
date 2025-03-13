#include "vector2.h"
#include "math.h"

void vec2_add(Vector2* vec, Vector2 add) {
    vec->x += add.x;
    vec->y += add.y;
}

void vec2_sub(Vector2* vec, Vector2 sub) {
    vec->x -= sub.x;
    vec->y -= sub.y;
}

void vec2_scale(Vector2* vec, float scalar) {
    vec->x *= scalar;
    vec->y *= scalar;
}

float vec2_magnitude(Vector2* vec) {
    return sqrtf(vec->x * vec->x + vec->y * vec->y);
}

void vec2_normalize(Vector2* vec) {
    float mag = vec2_magnitude(vec);

    if (mag > 0.0001f) {
        float norm_mag = 1.0f / mag;
        vec->x *= norm_mag;
        vec->y *= norm_mag;
    }
}

void vec2_up(Vector2* vec, float amount) {
    vec->y -= amount;
}

void vec2_down(Vector2* vec, float amount) {
    vec->y += amount;
}

void vec2_left(Vector2* vec, float amount) {
    vec->x -= amount;
}

void vec2_right(Vector2* vec, float amount) {
    vec->x += amount;
}
