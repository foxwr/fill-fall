#ifndef GRID_H
#define GRID_H

#include "def.h"

#define SIZE 250

void placeOnGrid(u32 x, u32 y, u32 type);
struct Cell getCell(struct Coord crd, u32 x, u32 y);
void setCell(struct Coord crd, u32 x, u32 y, struct Cell cell);

#endif
