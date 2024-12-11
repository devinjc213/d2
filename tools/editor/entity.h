#ifndef ENTITY_H
#define ENTITY_H

#include "defs.h"

typedef struct {
  int x;
  int y;
  int w;
  int h;

  char src[MAX_PATH_LENGTH];

} Entity;

#endif
