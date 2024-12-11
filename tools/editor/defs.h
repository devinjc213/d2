#define TILE_SIZE 16
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) < (b) ? (b) : (a))

#define MAX_PATH_LENGTH 512;

typedef struct {
  float offset_x, offset_y;
  float scale;
} ZoomState;
