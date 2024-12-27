#include "grid.h"

#include "util.h"
#include "cell.h"

static int tick = 0;

EXPORT struct Cell grid[SIZE][SIZE];
EXPORT int getGridSize(void) { return SIZE; }

struct Cell getCell(struct Coord crd, u32 x, u32 y) {
  x += crd.x;
  y += crd.y;

  if(x < 0 || x >= SIZE || y < 0 || y >= SIZE)
    return (struct Cell) { .type = CELL_ROCK };
  else
    return grid[y][x];
}

void setCell(struct Coord crd, u32 x, u32 y, struct Cell cell) {
  x += crd.x;
  y += crd.y;

  if(x >= 0 && x < SIZE && y >= 0 && y < SIZE)
    grid[y][x] = cell;
}

EXPORT void placeOnGrid(u32 x, u32 y, u32 type) {
  struct Coord crd = { x, y };
  struct Cell cell = (struct Cell) {
    .type = type,
    .tick = tick,
    .shft = rand(),
    .reg1 = 0
  };

  setCell(crd, 0, 0, cell);
}

EXPORT void updateGrid(void) {
  tick = !tick;

  static u32 ri[SIZE] = { -1 };
  if(*ri == -1)
    for(int i = 0; i < SIZE; i++)
      ri[i] = i;

  struct Coord c;
  for(c.y = 0; c.y < SIZE; c.y++) {
    u32 i = rand() % SIZE;
    u32 j = rand() % SIZE;
    u32 a = ri[i];
    ri[i] = ri[j];
    ri[j] = a;

    for(u32 x = 0; x < SIZE; x++) {
      c.x = ri[x];
      struct Cell *cell = &grid[c.y][c.x];
      if(cell->tick != tick && cellUpdateFnTbl[cell->type]) {
        cell->tick = tick;
        cellUpdateFnTbl[cell->type](*cell, c);
      }
    }
  }
}