#ifndef GRID_H
#define GRID_H

#include "def.h"

#define SIZE 200

struct Cell getCell(struct Coord crd, int x, int y);
void setCell(struct Coord crd, int x, int y, struct Cell cell);

#endif
